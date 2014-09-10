//#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>
#include <signal.h>

#define VERSION "1.2.0"
#define MAX_PATH 256
#define DEFAULT_NB_LINES_BEFORE_FLUSH 50

const char* LOG_STARTED = "Log started\n";
const char* LOG_RESTARTED = "Log restarted\n";
static int reopenFlag = 0;

static int fileSize(const char* fileName)
{
  struct stat st;
  stat(fileName, &st);
  return st.st_size;
}

static void handleSignal(int signal)
{
  reopenFlag = 1;
}

static void logTime(FILE* logFile, int* nbWrittenBytesPtr,
    const int printToStdout)
{
  struct timeval tv;
  struct tm* ptm;
  char timeStr[40];
  long milliseconds;

  gettimeofday(&tv, NULL);
  ptm = localtime(&tv.tv_sec);
  strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", ptm);
  /* Compute milliseconds from microseconds. */
  milliseconds = tv.tv_usec / 1000;
  /* Print the formatted time, in seconds, followed by a decimal point
   and the milliseconds. */
  int timeLength = strlen(timeStr);
  snprintf(timeStr + timeLength, sizeof(timeStr) - timeLength, ".%03ld ",
      milliseconds);
  fwrite(timeStr, sizeof(char), timeLength + 5, logFile);
  *nbWrittenBytesPtr = *nbWrittenBytesPtr + timeLength + 5;
  if (printToStdout == 1)
  {
    printf("%s", timeStr);
  }
}

static int rotate(FILE** logFilePtr, const char** logFileNames,
    const int maxNbOfFiles)
{
  if (*logFilePtr)
  {
    fclose(*logFilePtr);
    *logFilePtr = NULL;
  }

  if (access(logFileNames[maxNbOfFiles - 1], F_OK) == 0)
  {
    unlink(logFileNames[maxNbOfFiles - 1]);
  }
  int index = maxNbOfFiles - 2;
  while (index >= 0)
  {
    if (access(logFileNames[index], F_OK) == 0)
    {
      rename(logFileNames[index], logFileNames[index + 1]);
    }
    --index;
  }

  *logFilePtr = fopen(logFileNames[0], "w");

  return 1;
}

static void usage(const char* execName)
{
  printf("%s " VERSION "\n", execName);
  printf(
      "Usage: %s filePath filePrefix maxFileSize maxNbOfFiles [prefixWithDate] [nbLinesBeforeFlush] [printLogStarted] [printToStdout]\n",
      execName);
  printf("Example: %s $ROOTACT/var/log/scl cvm 2048 4\n", execName);
  printf(
      "  filePath           - folder in which log files will be generated. This path must exists. Must end with '/'\n");
  printf("  filePrefix         - log file prefix\n");
  printf("  maxFileSize        - maximum log file size in Kb\n");
  printf("  maxNbOfFiles       - maximum number of log files\n");
  printf(
      "  prefixWithDate     - whether logs must be prefixed with the current date. Default is true\n");
  printf(
      "  nbLinesBeforeFlush - number of lines before flushing to the log file. Default is 10\n");
  printf(
      "  printLogStarted    - whether to print an entry when starting logs. Default is true\n");
  printf(
      "  printToStdout      - whether to print logs to stdout. Default is false\n");
  fflush(stdout);
}

static int logStdinWithRotation(const char* execName, const char** logFileNames,
    const int maxFileSizeInBytes, const int maxNbOfFiles,
    const int prefixWithDate, const int nbLinesBeforeFlush,
    const int printLogStarted, const int printToStdout)
{
  int character = 0;
  const char* logFileName = logFileNames[0];
  FILE* logFile = NULL;

  char str[] = { 0, 0 };
  int nbWrittenBytes = 0;
  int nbWrittenLines = 0;
  int newLine = 1;

  // Check it is exists and append instead otherwise create file
  if (access(logFileName, F_OK) == 0)
  {
    // log file 0 exists tests its size
    int nbWrittenBytes = fileSize(logFileName);
    if (nbWrittenBytes < maxFileSizeInBytes)
    {
      // log file 0 has not reached the maximum size, open it in append mode
      logFile = fopen(logFileName, "a");
    }
    else
    {
      // log file 0 is full, rotate
      rotate(&logFile, logFileNames, maxNbOfFiles);
      nbWrittenBytes = 0;
    }
  }
  else
  {
    logFile = fopen(logFileName, "w");
  }

  if (!logFile)
  {
    printf("%s: error while opening file %s\n", execName, logFileName);
    return EXIT_FAILURE;
  }

  if (printLogStarted == 1)
  {
    logTime(logFile, &nbWrittenBytes, printToStdout);
    fwrite(LOG_STARTED, sizeof(char), strlen(LOG_STARTED), logFile);
    if (printToStdout == 1)
    {
      printf("%s", LOG_STARTED);
    }
    nbWrittenBytes += strlen(LOG_STARTED);
    fflush(logFile);
  }

  while ((character = getchar()) != -1)
  {
    if (newLine == 1)
    {
      if (reopenFlag == 1)
      {
        reopenFlag = 0;
        fclose(logFile);
        logFile = fopen(logFileName, "w");
        if (!logFile)
        {
          printf("%s: error while reopening file %s\n", execName, logFileName);
          return EXIT_FAILURE;
        }
        nbWrittenLines = 0;
        nbWrittenBytes = 0;
        if (printLogStarted == 1)
        {
          logTime(logFile, &nbWrittenBytes, printToStdout);
          fwrite(LOG_RESTARTED, sizeof(char), strlen(LOG_RESTARTED), logFile);
          if (printToStdout == 1)
          {
            printf("%s", LOG_RESTARTED);
          }
          nbWrittenBytes += strlen(LOG_RESTARTED);
          fflush(logFile);
        }
      }
      if (prefixWithDate)
      {
        logTime(logFile, &nbWrittenBytes, printToStdout);
      }
      newLine = 0;
    }
    str[0] = (char) character;
    if (fwrite(str, sizeof(char), 1, logFile) > 0)
    {
      if (printToStdout == 1)
      {
        printf("%s", str);
      }
      nbWrittenBytes += sizeof(char);
    }
    else
    {
      fprintf(stderr, "%s: error in writing log to %s\n", execName,
          logFileName);
      fflush(stdout);
      fflush(logFile);
      return EXIT_FAILURE;
    }

    if (character == '\n')
    {
      newLine = 1;
      if (printToStdout == 1)
      {
        fflush(stdout);
      }
      // Check if we need to reopen the file
      ++nbWrittenLines;
      if (nbWrittenLines == nbLinesBeforeFlush)
      {
        fflush(logFile);
        nbWrittenLines = 0;
      }
      if (nbWrittenBytes > maxFileSizeInBytes)
      {
        if (printToStdout == 1)
        {
          fprintf(stdout, "Rotation after %d written bytes\n", nbWrittenBytes);
        }

        rotate(&logFile, logFileNames, maxNbOfFiles);
        nbWrittenBytes = 0;
        nbWrittenLines = 0;
        if (logFile == NULL)
        {
          fprintf(stderr, "ERROR: logFile is NULL\n");
          return EXIT_FAILURE;
        }
      }
    }
  }

  if (logFile)
  {
    fclose(logFile);
  }

  return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
  if ((argc < 5) || (argc > 9))
  {
    usage(argv[0]);
    return EXIT_FAILURE;
  }

  char* execName = argv[0];
  char* filePath = argv[1];
  char* filePrefix = argv[2];
  int maxFileSizeInBytes = atoi(argv[3]) * 1024;
  int maxNbOfFiles = atoi(argv[4]);
  int prefixWithDate = 1;
  int nbLinesBeforeFlush = 10;
  int printLogStarted = 1;
  int printToStdout = 0;
  char* logFileNames[maxNbOfFiles];

  int fileNameLength = strlen(filePath) + strlen(filePrefix) + 9; // _XXX.log
  int i = 0;
  for (i = 0; i < maxNbOfFiles; ++i)
  {
    logFileNames[i] = (char*) malloc(sizeof(char) * fileNameLength);
    sprintf(logFileNames[i], "%s%s_%d.log", filePath, filePrefix, i);
  }

  if (argc > 5)
  {
    prefixWithDate = atoi(argv[5]);
  }
  if (argc > 6)
  {
    nbLinesBeforeFlush = atoi(argv[6]);
  }
  if (argc > 7)
  {
    printLogStarted = atoi(argv[7]);
  }
  if (argc > 8)
  {
    printToStdout = atoi(argv[8]);
  }

  // Disable stdout buffering
  setvbuf(stdout, (char *) NULL, _IONBF, 0);

  signal(SIGUSR1, handleSignal);

  logStdinWithRotation(execName, (const char**) logFileNames,
      maxFileSizeInBytes, maxNbOfFiles, prefixWithDate, nbLinesBeforeFlush,
      printLogStarted, printToStdout);

  return EXIT_SUCCESS;
}

