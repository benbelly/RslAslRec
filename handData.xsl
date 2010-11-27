<?xml version='1.0' encoding='UTF-8'?>
<xsl:stylesheet version='1.1' xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>
    <xsl:output method='text'/>
    <xsl:template match='ROOT'>
        <xsl:apply-templates select='Face'/>
        <xsl:apply-templates select='DominantHand'/>
        <xsl:apply-templates select='WeakHand'/>
    </xsl:template> 

    <xsl:template match='Face'>FACE: <xsl:value-of select='TopLeftX'/>,<xsl:value-of select='TopLeftY'/>,<xsl:value-of select='BottomRightX'/>,<xsl:value-of select='BottomRightY'/><xsl:text>
</xsl:text>
    </xsl:template> 

    <xsl:template match='DominantHand|WeakHand'>
        <xsl:if test='count(Pixels/singlepixel) > 0'><xsl:value-of select='substring-before(translate(name(.), "abcdefghijklmnopqrstuvwxyz", "ABCDEFGHIJKLMNOPQRSTUVWXYZ"), "HAND")' />: <xsl:for-each select='Pixels/singlepixel'><xsl:value-of select='x'/>, <xsl:value-of select='y'/><xsl:if test='not(. = ../singlepixel[last()])'>, </xsl:if>
            </xsl:for-each>
        </xsl:if><xsl:text>
</xsl:text>
    </xsl:template>

</xsl:stylesheet>
