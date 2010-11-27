<?xml version='1.0' encoding='UTF-8'?>
<xsl:stylesheet version='1.1' xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>
    <!--
         Accept a xml file that specifies hand pixels for an ASL video
         frame. Output the same data in a C++ friendlier format.
         Input:
             <ROOT>
                 <Face>
                   <TopLeftX>186</TopLeftX>
                   <TopLeftY>286</TopLeftY>
                   <BottomRightX>276</BottomRightX>
                   <BottomRightY>176</BottomRightY>
                 </Face>
                 <DominantHand>
                    <Pixels>
                        <singlepixel><x>1</x><y>1</y></singlepixel>
                        <singlepixel><x>2</x><y>1</y></singlepixel>
                    </Pixels>
                 </DominantHand>
                 <WeakHand>
                    <Pixels>
                        <singlepixel><x>3</x><y>1</y></singlepixel>
                        <singlepixel><x>4</x><y>1</y></singlepixel>
                    </Pixels>
                 </WeakHand>
             </ROOT>
        Output:
            FACE: 186 286 276 176
            DOMINANT: 1 1 2 1
            WEAK: 3 1 4 1
         -->
    <xsl:output method='text'/>
    <xsl:template match='ROOT'>
        <xsl:apply-templates select='Face'/>
        <xsl:apply-templates select='DominantHand'/>
        <xsl:apply-templates select='WeakHand'/>
    </xsl:template> 

    <xsl:template match='Face'>
        <xsl:text>FACE: </xsl:text>
        <xsl:value-of select='TopLeftX'/>
        <xsl:text> </xsl:text>
        <xsl:value-of select='TopLeftY'/>
        <xsl:text> </xsl:text>
        <xsl:value-of select='BottomRightX'/>
        <xsl:text> </xsl:text>
        <xsl:value-of select='BottomRightY'/>
        <xsl:text>
</xsl:text>
    </xsl:template> 

    <xsl:template match='DominantHand|WeakHand'>
        <xsl:if test='count(Pixels/singlepixel) > 0'>
            <!-- upper-case translation borrowed from
                 various internet sites
                 http://stackoverflow.com/questions/1207098/xslt-stylesheet-changing-text-to-upper-case
                 -->
            <xsl:value-of select='substring-before(translate(name(.),
                "abcdefghijklmnopqrstuvwxyz",
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"), "HAND")' />
            <xsl:text>: </xsl:text>
            <xsl:for-each select='Pixels/singlepixel'>
                <xsl:value-of select='x'/>
                <xsl:text> </xsl:text>
                <xsl:value-of select='y'/>
                <xsl:if test='not(. = ../singlepixel[last()])'>
                    <xsl:text> </xsl:text>
                </xsl:if>
            </xsl:for-each>
        </xsl:if>
        <xsl:text>
</xsl:text>
    </xsl:template>

</xsl:stylesheet>
