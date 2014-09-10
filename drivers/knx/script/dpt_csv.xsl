<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                              xmlns:str="http://exslt.org/strings"
                              xmlns:b="http://uri.actility.com/m2m/adaptor-knx"
                              exclude-result-prefixes="str b">
<xsl:output method="text" version="1.0" encoding="utf-8" indent="yes"/>
<xsl:template match="/">

<xsl:for-each select="b:DatapointTypes/b:DatapointType">
    <xsl:variable name="dpt_pri" select="@id" />
    <xsl:for-each select="b:DatapointSubtype">
        <xsl:variable name="dpt_sub" select="@id" />
        <xsl:variable name="name" select="@name" />
        <xsl:variable name="unit" select="@unit" />
        
        <xsl:if test="$unit != ''">
            <xsl:value-of select="$dpt_pri"/>.<xsl:value-of select='format-number($dpt_sub, "000")'/>;<xsl:value-of select="$name"/>;<xsl:value-of select="$unit"/>
            <xsl:text>&#xa;</xsl:text>
        </xsl:if>
    </xsl:for-each>        
</xsl:for-each>

</xsl:template>
</xsl:stylesheet>
