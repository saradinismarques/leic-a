JAVADIR=/home/sara/Documents/PO

JARNAME=woo-app

all:
	(cd src; javac -cp $(JAVADIR)/po-uuilib/po-uuilib.jar:$(JAVADIR)/project/woo-core/woo-core.jar -encoding UTF-8 `find . -name \*.java`)
	(cd src; jar cf ../$(JARNAME).jar `find . -name \*.class -o -name \*.java` )

clean:
	$(RM) $(JARNAME).jar `find . -name \*.class`
