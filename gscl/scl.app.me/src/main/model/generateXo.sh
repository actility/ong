#!/bin/sh

java -classpath ${HOME}/.m2/repository/com/sun/xsom/xsom/20110809/xsom-20110809.jar:${HOME}/.m2/repository/relaxngDatatype/relaxngDatatype/20020414/relaxngDatatype-20020414.jar:${HOME}/.m2/repository/com/actility/xo.xsd/1.2.0/xo.xsd-1.2.0.jar com.actility.xo.xsd.XoGenerator xsd xsd/patches xo


