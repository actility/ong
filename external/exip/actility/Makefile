
ROOT=$(ROOTACT)
#ROOT=$(HOME)

EXIP=$(ROOTACT)/exip
EXIPROCESSOR=$(ROOTACT)/ExiProcessor

CFLAGS += -I$(EXIP)/src/grammar/include -I$(EXIP)/include -I$(EXIP)/build/gcc/pc -I$(EXIP)/src/stringTables/include
LIB = $(EXIP)/bin/lib/libexip.a -lmxml -lpthread -lm
LDFLAGS += -L/usr/local/lib -s

all	: schemaConvert.x ALL_EXS xsitypeM2M.x sax.x xsitypeProduct.x dateTime.x

ALL_EXS : application.exs contentInstances.exs common.exs Product.exs Product_withRef.exs tc6_xml_v201.exs contentInstance.exs mime.exs subscription.exs subscriptions.exs

xsitypeM2M.x : xsitypeM2M.o
	$(CC) -o xsitypeM2M.x xsitypeM2M.o $(LDFLAGS) $(LIB)

xsitypeProduct.x : xsitypeProduct.o
	$(CC) -o xsitypeProduct.x xsitypeProduct.o $(LDFLAGS) $(LIB)

sax.x : sax.o
	$(CC) -o sax.x sax.o $(LDFLAGS) $(LIB)

dateTime.x : dateTime.o
	$(CC) -o dateTime.x dateTime.o $(LDFLAGS) $(LIB)

schemaConvert.x : schemaConvert.o
	$(CC) -o schemaConvert.x schemaConvert.o

lint :
	xmllint --schema Product.xsd product.xml

test :
	./xsitypeM2M.x
	./xsitypeM2M.x -s
	./xsitypeProduct.x
	./xsitypeProduct.x -s
	./sax.x -s Product.exs -f product.xml -I product
	./sax.x -s Product_withRef.exs -f product_withRef.xml -I product
	./sax.x -f STANDARD_FBS.xml -I project
	./sax.x -f plc.xml -I project
	./sax.x -f 0.exi
	./sax.x -f test.xml -I application -s common.exs,application.exs,contentInstances.exs,subscription.exs
	./sax.x -f test.xml
	./sax.x -f 102.xml -I subscription -s common.exs,subscription.exs,contentInstances.exs,contentInstance.exs,mime.exs
	./sax.x -f t2.xml -I subscription -s common.exs,contentInstances.exs,subscription.exs

testplc :
	./sax.x -f plc.xml -I project -o plc.exi
	./sax.x -f plc.exi -o __tmp__
	xmllint --format __tmp__ > plc.exi.xml
	rm __tmp__

clean	:
	@(rm *.a *.so *.o *.x *.exs 2>/dev/null || exit 0)

%.o : %.c
	$(CC) $(CFLAGS) $(PROF) -c $<

%.exs : %.xsd
	java -jar ${EXIPROCESSOR}/ExiProcessor.jar -header_options -preserve_prefixes -xml_in $< -exi_out $@
	@./schemaConvert.x < $@ > __temp
	@mv __temp $@
