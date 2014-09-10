<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                              xmlns:str="http://exslt.org/strings"
                              xmlns:foo="http://custom"
                              xmlns:b="http://knx.org/xml/project/11"
                              exclude-result-prefixes="str foo b">
<xsl:output method="xml" version="1.0" encoding="utf-8" indent="yes"/>

<!--
    Search function : Find CatalogItem node into CatalogSection nodes
-->
<xsl:function name="foo:findCatalogItem"> 
    <xsl:param name="n" as="node()"/>
    <xsl:param name="id" />
    <xsl:choose>
    <xsl:when test="$n/b:CatalogItem[contains(@Id, $id)]">
        <xsl:sequence select="$n/b:CatalogItem[contains(@Id, $id)]"/>
    </xsl:when>
    <xsl:otherwise>
        <xsl:if test="$n/b:CatalogSection">
            <xsl:for-each select="$n/b:CatalogSection">
                <xsl:sequence select="foo:findCatalogItem(self::node(), $id)"/>
          	</xsl:for-each>
        </xsl:if>
    </xsl:otherwise>
    </xsl:choose>
</xsl:function> 

<!--
    Search function : Find GroupRange node into GroupRanges nodes
-->
<xsl:function name="foo:findGroupRange"> 
  <xsl:param name="n" as="node()"/>
  <xsl:param name="id" /> 
  <xsl:choose>
    <xsl:when test="$n/b:GroupAddress[@Id = $id]">
         <xsl:sequence select="$n/b:GroupAddress[@Id = $id]"/>
    </xsl:when>
    <xsl:otherwise>
        <xsl:if test="$n/b:GroupRange">
            <xsl:for-each select="$n/b:GroupRange">
	            <xsl:sequence select="foo:findGroupRange(self::node(), $id)"/>
          	</xsl:for-each>
      	</xsl:if>
    </xsl:otherwise>
</xsl:choose>
</xsl:function> 


<!--
    Search function : Find GroupRange node into GroupRanges nodes
-->
<xsl:function name="foo:extractGwIP"> 
    <xsl:param name="id" />
    <xsl:variable name="token" select="substring-before($id, ';')"/>
    <xsl:variable name="key" select="substring-before($token, '=')"/>
    <xsl:variable name="val" select="substring-after($token, '=')"/>
    <xsl:choose>
        <xsl:when test="$key != 'IpAddr'">
            <xsl:variable name="next" select="substring-after($id, ';')"/>
            <xsl:value-of select="foo:extractGwIP($next)" />
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select='substring-before(substring-after($val, "&apos;"), "&apos;")' />
        </xsl:otherwise>
    </xsl:choose>
</xsl:function> 

<xsl:function name="foo:before-last-delimeter">
    <xsl:param name="s" />
    <xsl:param name="d" />
    <xsl:variable name="s-tokenized" select="tokenize($s, $d)"/>
    <xsl:value-of select="string-join(remove($s-tokenized, count($s-tokenized)),$d)"/>
</xsl:function>

<xsl:template match="/">
<xsl:variable name="graddress" select="/b:KNX/b:Project/b:Installations/b:Installation/b:GroupAddresses/b:GroupRanges" />

<knx xmlns="http://uri.actility.com/m2m/adaptor-knx"> 
<xsl:attribute name="gateway">
    <xsl:choose>
        <xsl:when test="/b:KNX/b:Project/b:Installations/b:Installation/b:BusAccess/@Parameter">
            <xsl:value-of select="foo:extractGwIP(/b:KNX/b:Project/b:Installations/b:Installation/b:BusAccess/@Parameter)"/>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="'0.0.0.0'"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:attribute>

<!-- 
    Generate device listing
-->
<xsl:for-each select="b:KNX/b:Project/b:Installations/b:Installation/b:Topology">
    <xsl:for-each select="b:Area">
        <xsl:variable name="area_addr" select="@Address" />
        <xsl:variable name="area_name" select="@Name" />
        <xsl:for-each select="b:Line">
            <xsl:variable name="line_addr" select="@Address" />
            <xsl:variable name="line_name" select="@Name" />
            <xsl:for-each select="b:DeviceInstance">
                <xsl:variable name="dev_addr" select="@Address" />
                <xsl:variable name="manufacturer" select="substring-before(@Hardware2ProgramRefId, '_')" />
                <xsl:variable name="firmware" select="substring-after(substring-after(substring-after(@Hardware2ProgramRefId, '_'), '_'), '-')" />
                <xsl:variable name="catalog" select="document(concat('./', $manufacturer, '/Catalog.xml'))/b:KNX/b:ManufacturerData/b:Manufacturer/b:Catalog" />
                <xsl:variable name="catalogItem" select="foo:findCatalogItem($catalog, $firmware)" />
                
                
                <!-- Write project parameters -->
                <device>
                
            	<xsl:attribute name="addr">
	                <xsl:value-of select="concat($area_addr, '.', $line_addr, '.', $dev_addr)"/>
                </xsl:attribute>
            	<xsl:attribute name="area">
	                <xsl:value-of select="$area_name"/>
                </xsl:attribute>
            	<xsl:attribute name="line">
	                <xsl:value-of select="$line_name"/>
                </xsl:attribute>
                <xsl:attribute name="manufacturer">
	                <xsl:value-of select="$manufacturer"/>
                </xsl:attribute>
                <xsl:attribute name="fw">
	                <xsl:value-of select="$firmware"/>
                </xsl:attribute>
                
                <!-- Write infos from Catalog (folder M-xxxx) -->
                <xsl:attribute name="name">
	                <xsl:value-of select="$catalogItem/@Name"/>
                </xsl:attribute>
                <xsl:attribute name="description">
	                <xsl:value-of select="$catalogItem/@VisibleDescription"/>
                </xsl:attribute>
                
                
                <!-- List Objects used by this device -->
                <xsl:for-each select="b:ComObjectInstanceRefs/b:ComObjectInstanceRef">
                <xsl:variable name="objectRefId" select="@RefId" />
                <xsl:variable name="deviceDefFile" select="concat(substring(@RefId, 0, 7), '/', substring-before(@RefId, '_O'), '.xml')" />
                <xsl:variable name="ComObjectID" select="document($deviceDefFile)/b:KNX/b:ManufacturerData/b:Manufacturer/b:ApplicationPrograms/b:ApplicationProgram/b:Static/b:ComObjectRefs/b:ComObjectRef[@Id = $objectRefId]/@RefId" />
                <xsl:variable name="ComObject" select="document($deviceDefFile)/b:KNX/b:ManufacturerData/b:Manufacturer/b:ApplicationPrograms/b:ApplicationProgram/b:Static/b:ComObjectTable/b:ComObject[@Id = $ComObjectID]"/>
                
                <!-- Ensure the object is used -->
                <xsl:if test="b:Connectors/b:Send or b:Connectors/b:Receive">
                    <object>
                        <!-- Extract Object ID, name & description -->
                        <xsl:attribute name="id">
	                        <xsl:value-of select="$ComObjectID"/>
                        </xsl:attribute>
                        
                        <xsl:attribute name="name">
                            <xsl:value-of select="document($deviceDefFile)/b:KNX/b:ManufacturerData/b:Manufacturer/b:Languages/b:Language[@Identifier = 'en-US']/b:TranslationUnit/b:TranslationElement[@RefId=$ComObjectID]/b:Translation[@AttributeName='Text']/@Text"/>
                        </xsl:attribute>
                        
                        <xsl:attribute name="description">
                            <xsl:value-of select="document($deviceDefFile)/b:KNX/b:ManufacturerData/b:Manufacturer/b:Languages/b:Language[@Identifier = 'en-US']/b:TranslationUnit/b:TranslationElement[@RefId=$ComObjectID]/b:Translation[@AttributeName='FunctionText']/@Text"/>
                        </xsl:attribute>
                                
                        <!-- Get DPT bit size -->
                        <xsl:variable name="sizeInBit"> 
                          <xsl:choose>
                            <xsl:when test="substring-after($ComObject/@ObjectSize,' ') = 'Bytes'">
                            <xsl:value-of select="number(substring-before($ComObject/@ObjectSize,' ')) * 8" /> 
                            </xsl:when>
                            <xsl:otherwise>
                            <xsl:value-of select="substring-before($ComObject/@ObjectSize,' ')" /> 
                            </xsl:otherwise>
                          </xsl:choose>
                        </xsl:variable>
                        <!--
                        <xsl:attribute name="sizeInBit">
                            <xsl:value-of select="$sizeInBit"/>
                        </xsl:attribute> foo:filterDPT
                        -->
                        
                        
                        <!-- DPT-ID is not always define in the project export,
                             Use knx_master.xml to get DPT with same bit lenght -->
                        
                        <xsl:variable name="master" select="document('knx_master.xml')/b:KNX/b:MasterData/b:DatapointTypes/b:DatapointType[@SizeInBit = $sizeInBit]" />
                        
                        <xsl:variable name="dptId"> 
                        <xsl:choose>
                            <xsl:when test="$ComObject/@DatapointType">
                                <xsl:value-of select="$ComObject/@DatapointType"/>
                            </xsl:when>
                            <xsl:when test="@DatapointType">
                                <xsl:value-of select="@DatapointType"/>
                            </xsl:when>
                            <xsl:otherwise>
                                <xsl:value-of select="''"/>
                            </xsl:otherwise>
                        </xsl:choose>
                        </xsl:variable>
                        
                        <xsl:attribute name="dptId">
                		<xsl:choose>
                		    <!-- DPT found -->
                            <xsl:when test="$dptId != ''">
                                <xsl:choose>
                                
                                    <!-- DPST (i.e. DPST-1-1) -->
                                    <xsl:when test="string-length($dptId) > 5">
	                                    <xsl:value-of select="concat(substring-before(substring-after($dptId, '-'), '-'), '.', format-number(number(substring-after(substring-after($dptId, '-'), '-')), '000'))"/>
                                    </xsl:when>
                                    
                                    <!-- DPT (i.e. DPT-1) -->
                                    <xsl:otherwise>
	                                    <xsl:value-of select="concat(substring-after($dptId, '-'), '.001')"/>
                                    </xsl:otherwise>
                                </xsl:choose>
                            </xsl:when>
                            
                            <!-- DPT not found, use a DPT with the same size -->
                            <xsl:otherwise>
                                <xsl:message terminate="no">
            Warning! The DPT-ID of a Object can NOT be found. Fallback on a default value, check the output file before use!
            ComObjectID = <xsl:value-of select="$ComObjectID"/>
            ---
                                </xsl:message>
                                <xsl:value-of select="concat($master[1]/@Number, '.', format-number($master[1]/b:DatapointSubtypes/b:DatapointSubtype[1]/@Number, '000'))"/>
                            </xsl:otherwise>
                        </xsl:choose>
                        </xsl:attribute>
                                                 
                        <!-- List group addr used by this object -->
                        <xsl:for-each select="b:Connectors/*">
                            <xsl:sort select="./@GroupAddressRefId"/>

                            <xsl:variable name="gaRef" select="@GroupAddressRefId" />
                            <xsl:variable name="gaItem" select="foo:findGroupRange($graddress, $gaRef)" />

                            <groupAddr>
                            
                                <xsl:attribute name="name">
	                                <xsl:value-of select="$gaItem/@Name"/>
                                </xsl:attribute>
                                <xsl:attribute name="addr">
	                                <xsl:value-of select="$gaItem/@Address"/>
                                </xsl:attribute>
                                <xsl:attribute name="send">
                                    <xsl:choose>
                                    <xsl:when test="name() = 'Send'">
	                                    <xsl:value-of select="'true'"/>
                                    </xsl:when>
                                    <xsl:otherwise>
	                                    <xsl:value-of select="'false'"/>
                                    </xsl:otherwise>
                                    </xsl:choose>
                                </xsl:attribute>
                            </groupAddr>
                        </xsl:for-each> 

                        </object>
                    </xsl:if> 
                    
                </xsl:for-each>
                </device>
                
            </xsl:for-each> 
        </xsl:for-each> 
    </xsl:for-each>
</xsl:for-each>

</knx>

</xsl:template>
</xsl:stylesheet>
