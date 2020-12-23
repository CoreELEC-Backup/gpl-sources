<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:exsl="http://exslt.org/common"
                xmlns:date="http://exslt.org/dates-and-times"
                xmlns:str="http://exslt.org/strings">
<xsl:output method="xml" omit-xml-declaration="yes" encoding='utf-8'/> 

<xsl:template match="/">
	<xsl:for-each select="//Sendung">
		<xsl:sort select="Datum"/>
		<xsl:sort select="Zeit"/>
		<xsl:variable name="EVENTID">
			<xsl:value-of select="SendungID"/>
		</xsl:variable>

		<xsl:variable name="SHOWVIEW">
			<xsl:value-of select="Showview"/>
		</xsl:variable>

		<xsl:variable name="PICS">
			<xsl:for-each select="Bild [@Kategorie='ImportBild']|*/Bild [@Kategorie='ImportBild']">
  			   <xsl:element name="icon">
			      <xsl:attribute name="src">file:///var/lib/epgsources/tvm2xmltv-img/<xsl:value-of select="BildDatei/BildURL"/></xsl:attribute>
                           </xsl:element>
			   <xsl:text>&#x0A;</xsl:text>
			</xsl:for-each>
		</xsl:variable>

		<xsl:variable name="TIPP">
			<xsl:for-each select="Bewertung">
			    <xsl:if test="substring(BewKategorieID , 0, 6) != 'tv14-'">
		  	        <xsl:text>[</xsl:text>
				<xsl:value-of select="BewKategorieID"/>
				<xsl:text>]</xsl:text>
		  	    </xsl:if>
			</xsl:for-each>
		</xsl:variable>
		
		<xsl:variable name="BEWERTUNG">
			<xsl:for-each select="*/Bewertung">
			        <xsl:if test="substring(BewKategorieID , 0, 6) != 'tv14-'">
					<xsl:element name="star-rating">
					<xsl:attribute name="system">
				        <xsl:value-of select="BewKategorieID"/>
					</xsl:attribute>
					<value><xsl:value-of select="Wertung"/><xsl:text>/3</xsl:text></value>
					</xsl:element>
					<xsl:text>&#x0A;</xsl:text>
				</xsl:if>
			</xsl:for-each>
		</xsl:variable>

		<xsl:variable name="CREW">
			<xsl:for-each select="*/Mensch [@Kategorie='Regisseur']">
				<director>
				<xsl:value-of select="Vorname"/>
				<xsl:text> </xsl:text>
				<xsl:value-of select="Name"/>
				</director><xsl:text>&#x0A;</xsl:text>
			</xsl:for-each>

			<xsl:for-each select="*/Mensch [@Kategorie='Schauspieler']">
				<actor>
				<xsl:value-of select="Vorname"/>
				<xsl:text> </xsl:text>
				<xsl:value-of select="Name"/>
				</actor><xsl:text>&#x0A;</xsl:text>
			</xsl:for-each>

			<xsl:for-each select="*/Mensch [@Kategorie='Drehbuch']">
				<writer>
				<xsl:value-of select="Vorname"/>
				<xsl:text> </xsl:text>
				<xsl:value-of select="Name"/>
				</writer><xsl:text>&#x0A;</xsl:text>
			</xsl:for-each>

			<xsl:for-each select="*/Mensch [@Kategorie='Produzent']">
				<producer>
				<xsl:value-of select="normalize-space(Vorname)"/>
				<xsl:text> </xsl:text>
				<xsl:value-of select="normalize-space(Name)"/>
				</producer><xsl:text>&#x0A;</xsl:text>
			</xsl:for-each>

			<xsl:for-each select="*/Mensch [@Kategorie='Musik']">
				<composer>
				<xsl:value-of select="Vorname"/>
				<xsl:text> </xsl:text>
				<xsl:value-of select="Name"/>
				</composer><xsl:text>&#x0A;</xsl:text>
			</xsl:for-each>	

			<xsl:for-each select="*/Mensch [@Kategorie='Schnitt']">
				<editor>
				<xsl:value-of select="Vorname"/>
				<xsl:text> </xsl:text>
				<xsl:value-of select="Name"/>
				</editor><xsl:text>&#x0A;</xsl:text>
			</xsl:for-each>

			<xsl:for-each select="*/Mensch [@Kategorie='Gast']">
				<guest>
				<xsl:value-of select="Vorname"/>
				<xsl:text> </xsl:text>
				<xsl:value-of select="Name"/>
				</guest><xsl:text>&#x0A;</xsl:text>
			</xsl:for-each>
				 
		</xsl:variable>

		<xsl:variable name="INHALT">
			<xsl:call-template name="getlongest">
				<xsl:with-param name="nodeset" select="*/Text[@Kategorie='Inhalt']/Text|Text[@Kategorie='Inhalt']/Text" />
			</xsl:call-template>
		</xsl:variable>

        <xsl:variable name="AUDIO">
            <xsl:if test="*/dolby">
                <xsl:text>dolby</xsl:text>
            </xsl:if>
            <xsl:if test="*/dolbydigital">
	            <xsl:text>dolby digital</xsl:text>
            </xsl:if>
            <xsl:if test="*/stereo">
                <xsl:text>stereo</xsl:text>
            </xsl:if>
            <xsl:if test="*/surroundsound">
                <xsl:text>surround</xsl:text>
            </xsl:if>
            <xsl:if test="*/zweikanal">
                <xsl:text>bilingual</xsl:text>
            </xsl:if>
        </xsl:variable>

        <xsl:variable name="VIDEO">

            <xsl:if test="*/sw">
            <colour>no</colour><xsl:text>&#x0A;</xsl:text>
            </xsl:if>

            <xsl:if test="*/breitwand">
            <aspect>16:9</aspect><xsl:text>&#x0A;</xsl:text>
            </xsl:if>

            <xsl:if test="*/hdtv">
            <quality>HDTV</quality><xsl:text>&#x0A;</xsl:text>
            </xsl:if>

        </xsl:variable>

		<xsl:variable name="GENRE">
			<xsl:choose>
			<xsl:when test="substring(*/GenreText,string-length(*/GenreText)-3) = 'film'">
				<xsl:value-of select="substring(*/GenreText,1,string-length(*/GenreText)-4)"/>
            </xsl:when>
			<xsl:otherwise>
				<xsl:choose>
				<xsl:when test="substring(*/GenreText,string-length(*/GenreText)-4) = '-Film'">
					<xsl:value-of select="substring(*/GenreText,1,string-length(*/GenreText)-5)"/>
                </xsl:when>
				<xsl:otherwise>
					<xsl:choose>
					<xsl:when test="substring(*/GenreText,string-length(*/GenreText)-4) = 'serie'">
						<xsl:value-of select="substring(*/GenreText,1,string-length(*/GenreText)-5)"/>
                	</xsl:when>
					<xsl:otherwise>
						<xsl:choose>
						<xsl:when test="substring(*/GenreText,string-length(*/GenreText)-5) = '-Serie'">
							<xsl:value-of select="substring(*/GenreText,1,string-length(*/GenreText)-6)"/>
            	    	</xsl:when>
						<xsl:otherwise>
							<xsl:choose>
							<xsl:when test="substring(*/GenreText,string-length(*/GenreText)-4) = 'reihe'">
								<xsl:value-of select="substring(*/GenreText,1,string-length(*/GenreText)-5)"/>
                			</xsl:when>
							<xsl:otherwise>
								<xsl:choose>
								<xsl:when test="substring(*/GenreText,string-length(*/GenreText)-3) = 'show'">
									<xsl:value-of select="substring(*/GenreText,1,string-length(*/GenreText)-4)"/>
            					</xsl:when>
								<xsl:otherwise>
									<xsl:choose>
									<xsl:when test="substring(*/GenreText,string-length(*/GenreText)-4) = '-Show'">
										<xsl:value-of select="substring(*/GenreText,1,string-length(*/GenreText)-5)"/>
                					</xsl:when>
									<xsl:otherwise>
										<xsl:value-of select="*/GenreText"/>
									</xsl:otherwise>
									</xsl:choose>
								</xsl:otherwise>
								</xsl:choose>
							</xsl:otherwise>
							</xsl:choose>
						</xsl:otherwise>
						</xsl:choose>
					</xsl:otherwise>
					</xsl:choose>
				</xsl:otherwise>
				</xsl:choose>
			</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>

		<xsl:variable name="KATEGORY">
			<xsl:value-of select="*/RZPKategorieID"/>	
		</xsl:variable>

<xsl:variable name="GENREDVB">

<xsl:choose>

<xsl:when test ="$KATEGORY = 'AA'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Dokumentation'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Reportage'"><xsl:text>G 81</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>

<xsl:when test ="$KATEGORY = 'Abenteuerserie'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Abenteuer'"><xsl:text>G 12,15</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Actionserie'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Action'"><xsl:text>G 10,15</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Arzt-Hospital'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Arzt'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Krankenhaus'"><xsl:text>G 10</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Bildung'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Doku'"><xsl:text>G 81,23</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Kindermagazin'"><xsl:text>G 54</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Magazin'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Porträt'"><xsl:text>G 83</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Reportage'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Wirtschaftsmagazin'"><xsl:text>G 82</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Wissenschaftsmagazin'"><xsl:text>G 92</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Comedy'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Comedy'"><xsl:text>G 14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Dramedy'"><xsl:text>G 14,10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Sitcom'"><xsl:text>G 14</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Dokumentarfilm'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Dokumentar'"><xsl:text>G 23,81</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Esskultur'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Dokumentation'"><xsl:text>G 81,A5</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Doku-Soap'"><xsl:text>G 15,A5</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Familienserie'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Anwalts'"><xsl:text>G 15,11</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Daily Soap'"><xsl:text>G 15</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Drama'"><xsl:text>G 15,10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Dramedy'"><xsl:text>G 15,10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Familien'"><xsl:text>G 15</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Gerichts'"><xsl:text>G 15</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Telenovela'"><xsl:text>G 15</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Fernsehfilm'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Abenteuer'"><xsl:text>G 12</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Actiondrama'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Actionkomödie'"><xsl:text>G 14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Drama'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Ehekomödie'"><xsl:text>G 14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Erotikdrama'"><xsl:text>G 18</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Familienkomödie'"><xsl:text>G 14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Fantasy'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Heimat'"><xsl:text>G 15</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Jugend'"><xsl:text>G 53</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Katastrophen'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Katastrophenthriller'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Komödie'"><xsl:text>G 14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Krimi'"><xsl:text>G 11</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Liebes'"><xsl:text>G 16</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Liebesdrama'"><xsl:text>G 16</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Liebeskomödie'"><xsl:text>G 16,14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Literaturverfilmung'"><xsl:text>G 75,10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Märchen'"><xsl:text>G 50</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Melodram'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Politdrama'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Politthriller'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Romantikkomödie'"><xsl:text>G 16,14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Science-Fiction-Action'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Science-Fiction'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Science-Fiction-Thriller'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Teenagerkomödie'"><xsl:text>G 53,14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Thriller'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Unterhaltungs'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Zeichentrick'"><xsl:text>G 55</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Forschung'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Dokumentation'"><xsl:text>G 90,23</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Magazin'"><xsl:text>G 90</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Wissenschaftsmagazin'"><xsl:text>G 92</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Geschichte'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Dokumentation'"><xsl:text>G 23</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'große Abendshow'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Casting'"><xsl:text>G 30</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Clip'"><xsl:text>G 30</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Comedy'"><xsl:text>G 30,14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Koch'"><xsl:text>G 30,A5</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Musikalische Reise'"><xsl:text>G 30,60</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Musik'"><xsl:text>G 30,60</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Show'"><xsl:text>G 30</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Spiel'"><xsl:text>G 31</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Heimatserie'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Heimat'"><xsl:text>G 15</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Jugendserie'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Jugend'"><xsl:text>G 53,15</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Kinder'"><xsl:text>G 52,15</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Tier'"><xsl:text>G 50,15</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Kabarett/Satire'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Kabarett'"><xsl:text>G 14</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Krimiserie'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Gerichts'"><xsl:text>G 10,15</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Krimi'"><xsl:text>G 10,15</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Polizei'"><xsl:text>G 10,15</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Real-Life-Krimi'"><xsl:text>G 10,15</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Kunst'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Clips'"><xsl:text>G 77</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Dokumentation'"><xsl:text>G 70,23,7A</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Gespräch'"><xsl:text>G 70,33</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Kindermagazin'"><xsl:text>G 70,50</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Kinomagazin'"><xsl:text>G 76</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Kulturmagazin'"><xsl:text>G 7A</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Kurzfilmmagazin'"><xsl:text>G 77</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Magazin'"><xsl:text>G 7A</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Medienmagazin'"><xsl:text>G 7A</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Porträt'"><xsl:text>G 70,83</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Kurz-Trickfilm'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Biografie'"><xsl:text>G 55,83</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Dokumentar'"><xsl:text>G 55,23</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Drama'"><xsl:text>G 55,10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Jugend'"><xsl:text>G 55,53</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Jugenddrama'"><xsl:text>G 55,53,10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Komödie'"><xsl:text>G 55,14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Musical'"><xsl:text>G 55,65</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Tanz'"><xsl:text>G 55,60</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Literatur'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Büchermagazin'"><xsl:text>G 75</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Diskussion'"><xsl:text>G 75,33</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Porträt'"><xsl:text>G 75,83</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Lotterie'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Lotterie'"><xsl:text>G 30</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Mode'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Dokumentation'"><xsl:text>G A0,23</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Doku-Soap'"><xsl:text>G A0,15</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Ratgeber'"><xsl:text>G A0</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Reportage'"><xsl:text>G A0</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Musik'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Dokumentation'"><xsl:text>G 60,23</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Konzert'"><xsl:text>G 60</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Musikmagazin'"><xsl:text>G 60</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Porträt'"><xsl:text>G 60</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Werbung'"><xsl:text>G A6,60</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Nachrichten'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Kindernachrichten'"><xsl:text>G 20,54</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Nachrichten'"><xsl:text>G 21</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Nachrichtenmagazin'"><xsl:text>G 22</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Sportnachrichten'"><xsl:text>G 20,41</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Wetter'"><xsl:text>G 21</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Natur'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Dokumentation'"><xsl:text>G A0,23</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Impressionen'"><xsl:text>G A0</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Naturdokumentation'"><xsl:text>G A0,23</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Politik'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Auslandsmagazin'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Diskussion'"><xsl:text>G 80,33</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Dokumentation'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Gespräch'"><xsl:text>G 80,33</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Magazin'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Politmagazin'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Reportage'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Wochenmagazin'"><xsl:text>G 81</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Pr.-Info'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Programminformation'"><xsl:text>G A6</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Quiz'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Quiz'"><xsl:text>G 31</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Spiel'"><xsl:text>G 31</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Realityshow'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Show'"><xsl:text>G 30</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Recht'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Doku'"><xsl:text>G 80,23</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Doku-Soap'"><xsl:text>G 80,15</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Kriminalmagazin'"><xsl:text>G 80,11</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Reportage'"><xsl:text>G 81</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Reisen'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Auslandsreportage'"><xsl:text>G A1</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Doku-Soap'"><xsl:text>G A1,15</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Impressionen'"><xsl:text>G A1</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Reisedokumentation'"><xsl:text>G A1,23</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Reisereportage'"><xsl:text>G A1</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Reportage'"><xsl:text>G A1</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Religion'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Ansprache'"><xsl:text>G 73</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Dokumentation'"><xsl:text>G 73,23</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Gespräch'"><xsl:text>G 73,33</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Gottesdienst'"><xsl:text>G 73</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Wochenmagazin'"><xsl:text>G 73</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Science-fiction'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Fantasy'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Märchen'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Mystery'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Science-Fiction'"><xsl:text>G 13</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Sendeschluß'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Sendeschluss'"><xsl:text>G A6</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Soziales'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Auslandsmagazin'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Auslandsreportage'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Boulevardmagazin'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Doku'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Dokumentation'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Doku-Soap'"><xsl:text>G 80,15</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Frühmagazin'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Gespräch'"><xsl:text>G 80,33</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Impressionen'"><xsl:text>G 80</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Kindermagazin'"><xsl:text>G 81,50</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Kulturmagazin'"><xsl:text>G 81,7A</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Magazin'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Mittagsmagazin'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Nachrichtenmagazin'"><xsl:text>G 81,22</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Porträt'"><xsl:text>G 83</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Ratgeber'"><xsl:text>G 82</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Reportage'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Wissensmagazin'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Wochenmagazin'"><xsl:text>G 81</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Spielfilm'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Abenteuer'"><xsl:text>G 12</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Abenteuerkomödie'"><xsl:text>G 12,14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Actionabenteuer'"><xsl:text>G 12</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Action'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Actiondrama'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Action-Fantasy'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Actionkomödie'"><xsl:text>G 14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Actionkrimi'"><xsl:text>G 11</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Actionthriller'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Agentenfilmparodie'"><xsl:text>G 14,11</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Agententhriller'"><xsl:text>G 11</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Animations'"><xsl:text>G 55</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Beziehungskomödie'"><xsl:text>G 16,14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Biografie'"><xsl:text>G 17</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Comicverfilmung'"><xsl:text>G 55</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Drama'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Fantasyabenteuer'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Fantasy'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Gaunerkomödie'"><xsl:text>G 11</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Gefängnisdrama'"><xsl:text>G 11</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Gesellschaftsdrama'"><xsl:text>G 17</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Historienabenteuer'"><xsl:text>G 12</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Historiendrama'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Horror-Action'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Horror'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Horrorkomödie'"><xsl:text>G 13,14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Horrorthriller'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Jugenddrama'"><xsl:text>G 17,53</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Justizdrama'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Katastrophen'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Kinderabenteuer'"><xsl:text>G 50</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Kinder'"><xsl:text>G 50</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Komödie'"><xsl:text>G 14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Kriegs'"><xsl:text>G 12</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Krimi'"><xsl:text>G 11</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Krimidrama'"><xsl:text>G 11</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Krimikomödie'"><xsl:text>G 11,14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Liebes'"><xsl:text>G 16</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Liebeskomödie'"><xsl:text>G 16,14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Literaturverfilmung'"><xsl:text>G 17</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Märchen'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Martial-Arts'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Mediensatire'"><xsl:text>G 14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Melodram'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Militärkomödie'"><xsl:text>G 14,12</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Musical'"><xsl:text>G 65</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Mysterythriller'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Piraten'"><xsl:text>G 12</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Politthriller'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Psychothriller'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Road Movie'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Romantikkomödie'"><xsl:text>G 16,14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Satire'"><xsl:text>G 14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Science-Fiction'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Science-Fiction-Horror'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Science-Fiction-Komödie'"><xsl:text>G 13,14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Science-Fiction-Thriller'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Sozialdrama'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Tanz'"><xsl:text>G 60</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Teenagerkomödie'"><xsl:text>G 14,53</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Thriller'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Tragikomödie'"><xsl:text>G 14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Trickfilmkomödie'"><xsl:text>G 14,55</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Western'"><xsl:text>G 12</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Westerndrama'"><xsl:text>G 12</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Westernkomödie'"><xsl:text>G 12,14</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Zeichentrick'"><xsl:text>G 55</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Zeichentrickkomödie'"><xsl:text>G 55,14</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Sport'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Basketball'"><xsl:text>G 40</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Boxen'"><xsl:text>G 4B</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Festakt'"><xsl:text>G 41</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Formel 1'"><xsl:text>G 47</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Fußball'"><xsl:text>G 43</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Leichtathletik'"><xsl:text>G 46</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Motorsport'"><xsl:text>G 47</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Sportmagazin'"><xsl:text>G 42</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Sportsendung'"><xsl:text>G 42</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Vorbericht'"><xsl:text>G 42,41</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Talkshow'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Talk'"><xsl:text>G 33</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Technik'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Automagazin'"><xsl:text>G A3</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Dokumentation'"><xsl:text>G 92</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Ratgeber'"><xsl:text>G 92</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Tiere'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Dokumentation'"><xsl:text>G A0,91</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Doku-Soap'"><xsl:text>G A0,15,91</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Reportage'"><xsl:text>G A0,91</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Tierdokumentation'"><xsl:text>G A0,91</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Tiermagazin'"><xsl:text>G A0,91</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Umwelt'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Dokumentation'"><xsl:text>G 91</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Naturdokumentation'"><xsl:text>G 91</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Umweltmagazin'"><xsl:text>G 91</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Unterhaltendes'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Doku-Soap'"><xsl:text>G 15</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Frühmagazin'"><xsl:text>G 15,22</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Impressionen'"><xsl:text>G 15</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Kindermagazin'"><xsl:text>G 54</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Medienmagazin'"><xsl:text>G 15</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Reportage'"><xsl:text>G 15</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Werbung'"><xsl:text>G A6</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Wochenmagazin'"><xsl:text>G 15</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Verkehr'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Automagazin'"><xsl:text>G A3</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Dokumentation'"><xsl:text>G A1,A3</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Doku-Soap'"><xsl:text>G A3,15</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Magazin'"><xsl:text>G A3</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Ratgeber'"><xsl:text>G A3</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Reportage'"><xsl:text>G A3</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="$KATEGORY = 'Wirtschaft'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Dokumentation'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Doku-Soap'"><xsl:text>G 15</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Reportage'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Wirtschaftsmagazin'"><xsl:text>G 81</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>

<xsl:when test ="$KATEGORY = 'Zeichentrick'">
<xsl:choose>
<xsl:when test ="$GENRE = 'Animations'"><xsl:text>G 55</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Anime'"><xsl:text>G 55</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Puppenspiel'"><xsl:text>G 55</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Puppentrick'"><xsl:text>G 55</xsl:text></xsl:when>
<xsl:when test ="$GENRE = 'Zeichentrick'"><xsl:text>G 55</xsl:text></xsl:when>
</xsl:choose>
</xsl:when>

</xsl:choose>

</xsl:variable>

		<xsl:variable name="ORIGTITLE">
			<xsl:value-of select="*/OriginalTitel"/>	
		</xsl:variable>

		<xsl:variable name="LAND">
			<xsl:value-of select="*/Land"/>	
		</xsl:variable>

		<xsl:variable name="JAHR">
			<xsl:value-of select="*/Jahr"/>	
		</xsl:variable>

		<xsl:variable name="FSK">
			<xsl:value-of select="*/FSK"/>
		</xsl:variable>

		<xsl:variable name="KURZKRITIK">
			<xsl:value-of select="Text[@Kategorie='Kurzkritik']/Text|*/Text[@Kategorie='Kurzkritik']/Text"/>
		</xsl:variable>

		<xsl:variable name="THEMEN">
			<xsl:for-each select="Text[@Kategorie='Thema']|*/Text[@Kategorie='Thema']">
			<xsl:value-of select="Text"/>
			<xsl:text>&#x0A;</xsl:text>
			</xsl:for-each>
		</xsl:variable>

<xsl:variable name="vps_iso8601">
<xsl:call-template name="date2UTC">
<xsl:with-param name="date" select="translate(VPS,' ','T')"/>
</xsl:call-template>
</xsl:variable>

<xsl:variable name="start_iso8601">
<xsl:call-template name="date2UTC">
<xsl:with-param name="date" select="concat(Datum,'T',Zeit)"/>
</xsl:call-template>
</xsl:variable> 

<xsl:variable name="start_xmltv">
<xsl:value-of select="concat(translate($start_iso8601,'-:ZT',''),' +0000')"/>
</xsl:variable>

<xsl:variable name="vps_xmltv">
<xsl:if test="string-length($vps_iso8601)"> 
<xsl:value-of select="concat(translate($vps_iso8601,'-:ZT',''),' +0000')"/>
</xsl:if>
</xsl:variable>

<xsl:element name="programme">
<xsl:attribute name="start">
<xsl:value-of select="$start_xmltv"/>
</xsl:attribute>


<xsl:if test="Dauer &lt;= 360">
<xsl:variable name="stop_iso8601">
<xsl:value-of select="date:add($start_iso8601,concat('PT',Dauer,'M'))"/>
</xsl:variable>
<xsl:variable name="stop_xmltv">
<xsl:value-of select="concat(translate($stop_iso8601,'-:ZT',''),' +0000')"/>
</xsl:variable>

<xsl:attribute name="stop">
<xsl:value-of select="$stop_xmltv"/>
</xsl:attribute>

</xsl:if>


<xsl:if test="string-length($vps_xmltv)">
<xsl:attribute name="vps-start">
<xsl:value-of select="$vps_xmltv"/>
</xsl:attribute>
</xsl:if>
<xsl:attribute name="channel">
<xsl:value-of select="$channelid"/>
</xsl:attribute>
<xsl:if test="string-length($SHOWVIEW)">
<xsl:attribute name="showview">
<xsl:value-of select="$SHOWVIEW"/>
</xsl:attribute>
</xsl:if>
<xsl:text>&#x0A;</xsl:text>
<xsl:if test="string-length($EVENTID)">
<xsl:comment> pid = <xsl:value-of select="$EVENTID"/><xsl:text> </xsl:text></xsl:comment><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<title lang="de"><xsl:value-of select="Titel"/></title><xsl:text>&#x0A;</xsl:text>
<xsl:if test="string-length($ORIGTITLE)">
<title><xsl:value-of select="$ORIGTITLE"/></title><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length(*/UnterTitel)">
<sub-title lang="de"><xsl:value-of select="*/UnterTitel"/></sub-title><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length($INHALT)">
<desc lang="de"><xsl:value-of select="$INHALT"/></desc><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length($THEMEN)">
<desc lang="de"><xsl:value-of select="$THEMEN"/></desc><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length($CREW)">
<credits><xsl:text>&#x0A;</xsl:text><xsl:copy-of select="$CREW"/></credits><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length($JAHR)">
<date><xsl:value-of select="$JAHR"/></date><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length($GENREDVB)">
<xsl:comment> content = <xsl:value-of select="$GENREDVB"/><xsl:text> </xsl:text></xsl:comment><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length($KATEGORY)">
<category lang="de"><xsl:value-of select="$KATEGORY"/></category><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length($GENRE)">
<category lang="de"><xsl:value-of select="$GENRE"/></category><xsl:text>&#x0A;</xsl:text>
</xsl:if>

<xsl:if test="string-length($PICS)">
<xsl:copy-of select="$PICS"/>
</xsl:if>
<xsl:if test="string-length($LAND)">
<country><xsl:value-of select="$LAND"/></country><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length($VIDEO)">
<video><xsl:text>&#x0A;</xsl:text><xsl:copy-of select="$VIDEO"/></video><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length($AUDIO)">
<audio><xsl:text>&#x0A;</xsl:text><stereo><xsl:value-of select="$AUDIO"/></stereo><xsl:text>&#x0A;</xsl:text></audio><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="tvpremiere">
<premiere /><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length($FSK)">
<rating system="FSK"><value><xsl:value-of select="$FSK"/></value></rating><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length($BEWERTUNG)">
<xsl:copy-of select="$BEWERTUNG"/>
</xsl:if>
<xsl:if test="string-length($KURZKRITIK)">
<review type="text"><xsl:value-of select="$KURZKRITIK"/></review><xsl:text>&#x0A;</xsl:text> 
</xsl:if>

</xsl:element>
<xsl:text>&#x0A;</xsl:text>
	</xsl:for-each>
</xsl:template>

<xsl:template name="getlongest">
  <xsl:param name="nodeset"/>
  <xsl:param name="longest"/>
  <xsl:choose>
    <xsl:when test="$nodeset">
      <xsl:choose>
        <xsl:when 
             test="string-length($nodeset[1]) > string-length($longest)">
          <xsl:call-template name="getlongest">
            <xsl:with-param name="nodeset" select="$nodeset[position() > 1]"/>
            <xsl:with-param name="longest" select="$nodeset[1]"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="getlongest">
            <xsl:with-param 
               name="nodeset" select="$nodeset[position() > 1]"/>
            <xsl:with-param name="longest" select="$longest"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$longest"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="date2UTC">
  <xsl:param name="date"/>

  <xsl:variable name="dststart">
    <xsl:value-of select="concat(date:year($date),'-03-',32-date:day-in-week(concat(date:year($date),'-03-31')),'T02:00:00')"/>
  </xsl:variable>

  <xsl:variable name="dstend">
    <xsl:value-of select="concat(date:year($date),'-10-',32-date:day-in-week(concat(date:year($date),'-10-31')),'T03:00:00')"/>
  </xsl:variable>

  <xsl:variable name="tz">
  <xsl:choose>
  <xsl:when test="date:seconds(date:difference($dststart,$date)) &gt;= 0">
    <xsl:choose>
    <xsl:when test="date:seconds(date:difference($date,$dstend)) &gt;= 0">-PT2H</xsl:when>
    <xsl:otherwise>-PT1H</xsl:otherwise>
    </xsl:choose>
  </xsl:when>
  <xsl:otherwise>-PT1H</xsl:otherwise>
  </xsl:choose>
  </xsl:variable>

 <xsl:value-of select="date:add($date,$tz)"/>
</xsl:template>


</xsl:stylesheet>
