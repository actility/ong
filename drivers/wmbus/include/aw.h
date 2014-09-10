
/*
 *	environment variable to get root directory of Actility/watteco datas
 *	getenv(3)
 */
#define	AW_DATA_DIR		"AW_DATA_DIR"

/*
 *	max sensors per router-controller
 */
#define	MAX_SSR_PER_RTR		10	// TODO test only

/*
 *	max applications per sensors
 */
#define	MAX_APP_PER_SSR		1	// only one application ...

/*
 *	max interfaces/clusters per sensors
 */
#define	MAX_ITF_PER_SSR		10

/*
 *	max attributs per cluster
 */
#define	MAX_ATT_PER_CLS		3

/*
 *	max members per attribut
 */
#define	MAX_MBR_PER_ATT		3

/*
 *	request http to router server to get sensors list
 *	strings to start and stop parsing response
 *	separator
 *	size max for response
 */
#define	HTTP_GET_ELEM_LIST		"GET / HTTP/1.1 \r\n"
#define	HTTP_PARSE_START		"<h2>Routes</h2>"
#define	HTTP_PARSE_END			"</body>"
#define	HTTP_PARSE_SEP			"<br>"
#define	HTTP_RESPONSE_SIZE		10000


char		*AwVersion();

char 		*AwGetPanPref(char *addr);
char		*AwGetPanAddr(char *pan);
int		AwParseRouter(char *res,char *tab[]);
int 		AwSensorDiscover(time_t *last,char *tbcur[],char *tbdel[],
								char *tbnew[]);
int		AwSensorSoftware(time_t *last,char *tbcur[],char *tbdel[],
								char *tbnew[]);


int		AwFileCmp(char *f1,char *f2);

unsigned char	*AwStrToBin(char *str,unsigned char bin[],int *lg);
char 		*AwBinToStr(unsigned char *bin,int lg,char dst[],int max);
void		AwPrintBin(FILE *f,unsigned char *buf,int lg);
