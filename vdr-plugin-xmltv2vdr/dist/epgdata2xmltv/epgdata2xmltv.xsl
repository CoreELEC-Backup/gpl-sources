<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:exsl="http://exslt.org/common"
                xmlns:date="http://exslt.org/dates-and-times"
                xmlns:str="http://exslt.org/strings">
<xsl:output method="xml" omit-xml-declaration="yes" encoding='utf-8'/> 

<xsl:template match="/">
<xsl:for-each select="//data[d2=$channelnum]">
<!-- <xsl:sort select="d4"/> --> 
<xsl:variable name="EVENTID">
<xsl:value-of select="d0"/>
</xsl:variable>

<xsl:variable name="INHALT">
<xsl:value-of select="translate(d22,'|','&#x0A;')"/>
</xsl:variable>

<xsl:variable name="THEMEN">
<xsl:value-of select="translate(d24,'|','&#x0A;')"/>
</xsl:variable>

<xsl:variable name="JAHR">
<xsl:value-of select="d33"/>
</xsl:variable>

<xsl:variable name="LAND">
<xsl:value-of select="translate(d32,'|','/')"/>
</xsl:variable>

<xsl:variable name="GENREDVB">
<xsl:choose>
<xsl:when test ="d25 = '101'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="d25 = '102'"><xsl:text>G 12</xsl:text></xsl:when>
<xsl:when test ="d25 = '103'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="d25 = '104'"><xsl:text>G 23</xsl:text></xsl:when>
<xsl:when test ="d25 = '105'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="d25 = '106'"><xsl:text>G 18</xsl:text></xsl:when>
<xsl:when test ="d25 = '108'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="d25 = '109'"><xsl:text>G 15</xsl:text></xsl:when>
<xsl:when test ="d25 = '110'"><xsl:text>G 14</xsl:text></xsl:when>
<xsl:when test ="d25 = '112'"><xsl:text>G 11</xsl:text></xsl:when>
<xsl:when test ="d25 = '113'"><xsl:text>G 76</xsl:text></xsl:when>
<xsl:when test ="d25 = '114'"><xsl:text>G 10</xsl:text></xsl:when>
<xsl:when test ="d25 = '115'"><xsl:text>G 10,60</xsl:text></xsl:when>
<xsl:when test ="d25 = '116'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="d25 = '117'"><xsl:text>G 16</xsl:text></xsl:when>
<xsl:when test ="d25 = '119'"><xsl:text>G 13</xsl:text></xsl:when>
<xsl:when test ="d25 = '121'"><xsl:text>G 11</xsl:text></xsl:when>
<xsl:when test ="d25 = '122'"><xsl:text>G 12</xsl:text></xsl:when>
<xsl:when test ="d25 = '123'"><xsl:text>G 10,55</xsl:text></xsl:when>
<xsl:when test ="d25 = '201'"><xsl:text>G 15</xsl:text></xsl:when>
<xsl:when test ="d25 = '202'"><xsl:text>G 15,12</xsl:text></xsl:when>
<xsl:when test ="d25 = '203'"><xsl:text>G 15</xsl:text></xsl:when>
<xsl:when test ="d25 = '205'"><xsl:text>G 15</xsl:text></xsl:when>
<xsl:when test ="d25 = '206'"><xsl:text>G 15,18</xsl:text></xsl:when>
<xsl:when test ="d25 = '207'"><xsl:text>G 15</xsl:text></xsl:when>
<xsl:when test ="d25 = '208'"><xsl:text>G 15,13</xsl:text></xsl:when>
<xsl:when test ="d25 = '210'"><xsl:text>G 15,14</xsl:text></xsl:when>
<xsl:when test ="d25 = '211'"><xsl:text>G 15</xsl:text></xsl:when>
<xsl:when test ="d25 = '212'"><xsl:text>G 15,11</xsl:text></xsl:when>
<xsl:when test ="d25 = '214'"><xsl:text>G 15,50</xsl:text></xsl:when>
<xsl:when test ="d25 = '216'"><xsl:text>G 15,13</xsl:text></xsl:when>
<xsl:when test ="d25 = '218'"><xsl:text>G 15,90</xsl:text></xsl:when>
<xsl:when test ="d25 = '219'"><xsl:text>G 15,13</xsl:text></xsl:when>
<xsl:when test ="d25 = '220'"><xsl:text>G 15,15</xsl:text></xsl:when>
<xsl:when test ="d25 = '221'"><xsl:text>G 15,11</xsl:text></xsl:when>
<xsl:when test ="d25 = '222'"><xsl:text>G 15,12</xsl:text></xsl:when>
<xsl:when test ="d25 = '223'"><xsl:text>G 15,55</xsl:text></xsl:when>
<xsl:when test ="d25 = '301'"><xsl:text>G 40</xsl:text></xsl:when>
<xsl:when test ="d25 = '331'"><xsl:text>G 4B</xsl:text></xsl:when>
<xsl:when test ="d25 = '332'"><xsl:text>G 45</xsl:text></xsl:when>
<xsl:when test ="d25 = '334'"><xsl:text>G 43</xsl:text></xsl:when>
<xsl:when test ="d25 = '335'"><xsl:text>G 41</xsl:text></xsl:when>
<xsl:when test ="d25 = '336'"><xsl:text>G 40</xsl:text></xsl:when>
<xsl:when test ="d25 = '337'"><xsl:text>G 46</xsl:text></xsl:when>
<xsl:when test ="d25 = '338'"><xsl:text>G 45</xsl:text></xsl:when>
<xsl:when test ="d25 = '339'"><xsl:text>G 47</xsl:text></xsl:when>
<xsl:when test ="d25 = '340'"><xsl:text>G 40</xsl:text></xsl:when>
<xsl:when test ="d25 = '341'"><xsl:text>G 44</xsl:text></xsl:when>
<xsl:when test ="d25 = '342'"><xsl:text>G 48</xsl:text></xsl:when>
<xsl:when test ="d25 = '343'"><xsl:text>G 49</xsl:text></xsl:when>
<xsl:when test ="d25 = '344'"><xsl:text>G 45</xsl:text></xsl:when>
<xsl:when test ="d25 = '345'"><xsl:text>G 46</xsl:text></xsl:when>
<xsl:when test ="d25 = '346'"><xsl:text>G 45</xsl:text></xsl:when>
<xsl:when test ="d25 = '347'"><xsl:text>G 40</xsl:text></xsl:when>
<xsl:when test ="d25 = '348'"><xsl:text>G 40,23</xsl:text></xsl:when>
<xsl:when test ="d25 = '401'"><xsl:text>G 30</xsl:text></xsl:when>
<xsl:when test ="d25 = '406'"><xsl:text>G 30</xsl:text></xsl:when>
<xsl:when test ="d25 = '418'"><xsl:text>G 30</xsl:text></xsl:when>
<xsl:when test ="d25 = '450'"><xsl:text>G 30</xsl:text></xsl:when>
<xsl:when test ="d25 = '451'"><xsl:text>G 30</xsl:text></xsl:when>
<xsl:when test ="d25 = '452'"><xsl:text>G 31</xsl:text></xsl:when>
<xsl:when test ="d25 = '453'"><xsl:text>G 33</xsl:text></xsl:when>
<xsl:when test ="d25 = '454'"><xsl:text>G 30</xsl:text></xsl:when>
<xsl:when test ="d25 = '455'"><xsl:text>G A6</xsl:text></xsl:when>
<xsl:when test ="d25 = '456'"><xsl:text>G A5</xsl:text></xsl:when>
<xsl:when test ="d25 = '457'"><xsl:text>G A2</xsl:text></xsl:when>
<xsl:when test ="d25 = '501'"><xsl:text>G 90</xsl:text></xsl:when>
<xsl:when test ="d25 = '560'"><xsl:text>G 96</xsl:text></xsl:when>
<xsl:when test ="d25 = '561'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="d25 = '564'"><xsl:text>G A4</xsl:text></xsl:when>
<xsl:when test ="d25 = '565'"><xsl:text>G A3</xsl:text></xsl:when>
<xsl:when test ="d25 = '566'"><xsl:text>G 21</xsl:text></xsl:when>
<xsl:when test ="d25 = '567'"><xsl:text>G 91</xsl:text></xsl:when>
<xsl:when test ="d25 = '568'"><xsl:text>G 80</xsl:text></xsl:when>
<xsl:when test ="d25 = '569'"><xsl:text>G 82</xsl:text></xsl:when>
<xsl:when test ="d25 = '570'"><xsl:text>G A1</xsl:text></xsl:when>
<xsl:when test ="d25 = '571'"><xsl:text>G 80</xsl:text></xsl:when>
<xsl:when test ="d25 = '572'"><xsl:text>G 90</xsl:text></xsl:when>
<xsl:when test ="d25 = '573'"><xsl:text>G 81</xsl:text></xsl:when>
<xsl:when test ="d25 = '601'"><xsl:text>G 60</xsl:text></xsl:when>
<xsl:when test ="d25 = '680'"><xsl:text>G 64</xsl:text></xsl:when>
<xsl:when test ="d25 = '681'"><xsl:text>G 62</xsl:text></xsl:when>
<xsl:when test ="d25 = '682'"><xsl:text>G 65</xsl:text></xsl:when>
<xsl:when test ="d25 = '683'"><xsl:text>G 61</xsl:text></xsl:when>
<xsl:when test ="d25 = '684'"><xsl:text>G 63</xsl:text></xsl:when>
<xsl:when test ="d25 = '685'"><xsl:text>G 61</xsl:text></xsl:when>
<xsl:when test ="d25 = '686'"><xsl:text>G 61</xsl:text></xsl:when>
<xsl:when test ="d25 = '687'"><xsl:text>G 61</xsl:text></xsl:when>
<xsl:when test ="d25 = '688'"><xsl:text>G 60,30</xsl:text></xsl:when>
<xsl:when test ="d25 = '689'"><xsl:text>G 60,83</xsl:text></xsl:when>
<xsl:when test ="d25 = '690'"><xsl:text>G 60,71</xsl:text></xsl:when>
<xsl:when test ="d25 = '691'"><xsl:text>G 60,76</xsl:text></xsl:when>
<xsl:when test ="d25 = '692'"><xsl:text>G 60,70</xsl:text></xsl:when>
<xsl:when test ="d25 = '701'"><xsl:text>G 50</xsl:text></xsl:when>
<xsl:when test ="d25 = '790'"><xsl:text>G 50,10</xsl:text></xsl:when>
<xsl:when test ="d25 = '791'"><xsl:text>G 50,21</xsl:text></xsl:when>
<xsl:when test ="d25 = '792'"><xsl:text>G 50</xsl:text></xsl:when>
<xsl:when test ="d25 = '793'"><xsl:text>G 50,30</xsl:text></xsl:when>
<xsl:when test ="d25 = '795'"><xsl:text>G 55</xsl:text></xsl:when>
<xsl:when test ="d25 = '796'"><xsl:text>G 55</xsl:text></xsl:when>
</xsl:choose>
</xsl:variable>

<xsl:variable name="GENRE">
<xsl:choose>
<xsl:when test ="d10 = '100'">
<xsl:choose>
<xsl:when test ="d25 = '101'">
<xsl:text>Spielfilm / Verschiedenes</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '102'">
<xsl:text>Spielfilm / Abenteuer</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '103'">
<xsl:text>Spielfilm / Action</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '104'">
<xsl:text>Spielfilm / Dokumentarfilm</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '105'">
<xsl:text>Spielfilm / Drama</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '106'">
<xsl:text>Spielfilm / Erotik</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '108'">
<xsl:text>Spielfilm / Fantasy</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '109'">
<xsl:text>Spielfilm / Heimat</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '110'">
<xsl:text>Spielfilm / Humor</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '112'">
<xsl:text>Spielfilm / Krimi</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '113'">
<xsl:text>Spielfilm / Kultur</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '114'">
<xsl:text>Spielfilm / Kurzfilm</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '115'">
<xsl:text>Spielfilm / Musik</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '116'">
<xsl:text>Spielfilm / Mystery+Horror</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '117'">
<xsl:text>Spielfilm / Romantik/Liebe</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '119'">
<xsl:text>Spielfilm / Science Fiction</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '121'">
<xsl:text>Spielfilm / Thriller</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '122'">
<xsl:text>Spielfilm / Western</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '123'">
<xsl:text>Spielfilm / Zeichentrick</xsl:text>
</xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="d10 = '200'">
<xsl:choose>
<xsl:when test ="d25 = '201'">
<xsl:text>Serie / Verschiedenes</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '202'">
<xsl:text>Serie / Abenteuer</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '203'">
<xsl:text>Serie / Action</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '205'">
<xsl:text>Serie / Drama</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '206'">
<xsl:text>Serie / Erotik</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '207'">
<xsl:text>Serie / Familie</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '208'">
<xsl:text>Serie / Fantasy</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '210'">
<xsl:text>Serie / Humor</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '211'">
<xsl:text>Serie / Krankenhaus</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '212'">
<xsl:text>Serie / Krimi</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '214'">
<xsl:text>Serie / Jugend</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '216'">
<xsl:text>Serie / Mystery+Horror</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '218'">
<xsl:text>Serie / Reality</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '219'">
<xsl:text>Serie / Science Fiction</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '220'">
<xsl:text>Serie / Soap</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '221'">
<xsl:text>Serie / Thriller</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '222'">
<xsl:text>Serie / Western</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '223'">
<xsl:text>Serie / Zeichentrick</xsl:text>
</xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="d10 = '300'">
<xsl:choose>
<xsl:when test ="d25 = '301'">
<xsl:text>Sport / Verschiedenes</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '331'">
<xsl:text>Sport / Boxen</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '332'">
<xsl:text>Sport / Eishockey</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '334'">
<xsl:text>Sport / Fussball</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '335'">
<xsl:text>Sport / Olympia</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '336'">
<xsl:text>Sport / Golf</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '337'">
<xsl:text>Sport / Gymnastik</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '338'">
<xsl:text>Sport / Handball</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '339'">
<xsl:text>Sport / Motorsport</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '340'">
<xsl:text>Sport / Radsport</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '341'">
<xsl:text>Sport / Tennis</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '342'">
<xsl:text>Sport / Wassersport</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '343'">
<xsl:text>Sport / Wintersport</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '344'">
<xsl:text>Sport / US-Sport</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '345'">
<xsl:text>Sport / Leichtathletik</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '346'">
<xsl:text>Sport / Volleyball</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '347'">
<xsl:text>Sport / Extremsport</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '348'">
<xsl:text>Sport / Reportagen</xsl:text>
</xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="d10 = '400'">
<xsl:choose>
<xsl:when test ="d25 = '401'">
<xsl:text>Show / Verschiedenes</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '406'">
<xsl:text>Show / Erotik</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '418'">
<xsl:text>Show / Reality</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '450'">
<xsl:text>Show / Comedy</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '451'">
<xsl:text>Show / Familien-Show</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '452'">
<xsl:text>Show / Spielshows</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '453'">
<xsl:text>Show / Talkshows</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '454'">
<xsl:text>Show / Gerichtsshow</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '455'">
<xsl:text>Show / Homeshopping</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '456'">
<xsl:text>Show / Kochshow</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '457'">
<xsl:text>Show / Heimwerken</xsl:text>
</xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="d10 = '500'">
<xsl:choose>
<xsl:when test ="d25 = '501'">
<xsl:text>Information / Verschiedenes</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '560'">
<xsl:text>Information / Geschichte</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '561'">
<xsl:text>Information / Magazin</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '564'">
<xsl:text>Information / Gesundheit</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '565'">
<xsl:text>Information / Motor+Verkehr</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '566'">
<xsl:text>Information / Nachrichten</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '567'">
<xsl:text>Information / Natur</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '568'">
<xsl:text>Information / Politik</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '569'">
<xsl:text>Information / Ratgeber</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '570'">
<xsl:text>Information / Reise</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '571'">
<xsl:text>Information / Wirtschaft</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '572'">
<xsl:text>Information / Wissen</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '573'">
<xsl:text>Information / Dokumentation</xsl:text>
</xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="d10 = '600'">
<xsl:choose>
<xsl:when test ="d25 = '601'">
<xsl:text>Kultur + Musik / Verschiedenes</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '680'">
<xsl:text>Kultur + Musik / Jazz</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '681'">
<xsl:text>Kultur + Musik / Klassik</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '682'">
<xsl:text>Kultur + Musik / Musical</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '683'">
<xsl:text>Kultur + Musik / Rock</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '684'">
<xsl:text>Kultur + Musik / Volksmusik</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '685'">
<xsl:text>Kultur + Musik / Alternative</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '686'">
<xsl:text>Kultur + Musik / Pop</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '687'">
<xsl:text>Kultur + Musik / Clips</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '688'">
<xsl:text>Kultur + Musik / Show</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '689'">
<xsl:text>Kultur + Musik / Interview</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '690'">
<xsl:text>Kultur + Musik / Theater</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '691'">
<xsl:text>Kultur + Musik / Kino</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '692'">
<xsl:text>Kultur + Musik / Kultur</xsl:text>
</xsl:when>
</xsl:choose>
</xsl:when>
<xsl:when test ="d10 = '700'">
<xsl:choose>
<xsl:when test ="d25 = '701'">
<xsl:text>Kinder / Verschiedenes</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '790'">
<xsl:text>Kinder / Filme</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '791'">
<xsl:text>Kinder / Nachrichten</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '792'">
<xsl:text>Kinder / Serien</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '793'">
<xsl:text>Kinder / Shows</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '795'">
<xsl:text>Kinder / Zeichentrick</xsl:text>
</xsl:when>
<xsl:when test ="d25 = '796'">
<xsl:text>Kinder / Anime</xsl:text>
</xsl:when>
</xsl:choose>
</xsl:when>
</xsl:choose>
</xsl:variable>

<xsl:variable name="EPISODE">
<xsl:if test="d26 &gt; 0"><xsl:value-of select="d26 - 1"/></xsl:if>
</xsl:variable>

<xsl:variable name="PICS">
<xsl:if test="string-length(d38)">
<xsl:element name="icon">
<xsl:attribute name="src">file:///var/lib/epgsources/epgdata2xmltv-img/<xsl:value-of select="d38"/></xsl:attribute>
</xsl:element>
<xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length(d39)">
<xsl:element name="icon">
<xsl:attribute name="src">file:///var/lib/epgsources/epgdata2xmltv-img/<xsl:value-of select="d39"/></xsl:attribute>
</xsl:element>
<xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length(d40)">
<xsl:element name="icon">
<xsl:attribute name="src">file:///var/lib/epgsources/epgdata2xmltv-img/<xsl:value-of select="d40"/></xsl:attribute>
</xsl:element>
<xsl:text>&#x0A;</xsl:text>
</xsl:if>
</xsl:variable>

<xsl:variable name="CREW">
<xsl:if test="string-length(d36)">
<xsl:call-template name="output-tokens">
<xsl:with-param name="list"><xsl:value-of select="d36"/></xsl:with-param>
<xsl:with-param name="tag">director</xsl:with-param>
<xsl:with-param name="delimiter">|</xsl:with-param>
</xsl:call-template>
</xsl:if>

<xsl:if test="string-length(d37)">
<xsl:call-template name="output-tokens">
<xsl:with-param name="list"><xsl:value-of select="d37"/></xsl:with-param>
<xsl:with-param name="tag">actor</xsl:with-param>
<xsl:with-param name="delimiter"> - </xsl:with-param>
</xsl:call-template>
</xsl:if>

<xsl:if test="string-length(d34)">
<xsl:call-template name="output-tokens">
<xsl:with-param name="list"><xsl:value-of select="d34"/></xsl:with-param>
<xsl:with-param name="tag">presenter</xsl:with-param>
<xsl:with-param name="delimiter">|</xsl:with-param>
</xsl:call-template>
</xsl:if>

<xsl:if test="string-length(d35)">
<xsl:call-template name="output-tokens">
<xsl:with-param name="list"><xsl:value-of select="d35"/></xsl:with-param>
<xsl:with-param name="tag">guest</xsl:with-param>
<xsl:with-param name="delimiter"> - </xsl:with-param>
</xsl:call-template>
</xsl:if>

</xsl:variable>

<xsl:variable name="VIDEO">

<xsl:if test="d29='1'">
<aspect>16:9</aspect><xsl:text>&#x0A;</xsl:text>
</xsl:if>

<xsl:if test="d11='1'">
<colour>no</colour><xsl:text>&#x0A;</xsl:text>
</xsl:if>

</xsl:variable>

<xsl:variable name="AUDIO">
<xsl:choose>
<xsl:when test="d28='1'">
    <xsl:text>dolby digital</xsl:text>
</xsl:when>
<!--
<xsl:when test="d12='1'">
    <xsl:text>bilingual</xsl:text>
</xsl:when>
-->
<xsl:when test="d27='1'">
    <xsl:text>stereo</xsl:text>
</xsl:when>
</xsl:choose>
</xsl:variable>

<xsl:variable name="STARRATING">
<xsl:if test="d30 &gt; 0">
<star-rating><value><xsl:value-of select="d30"/><xsl:text>/5</xsl:text></value></star-rating><xsl:text>&#x0A;</xsl:text>
</xsl:if>
</xsl:variable>

<xsl:variable name="TIPP">
<xsl:if test="d18=1">
<star-rating system="TagesTipp"><value>1/1</value></star-rating><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="d18=2">
<star-rating system="TopTipp"><value>1/1</value></star-rating><xsl:text>&#x0A;</xsl:text>
</xsl:if>
</xsl:variable>

<xsl:variable name="vps_iso8601">
<xsl:if test="string-length(d8)">
<xsl:call-template name="date2UTC">
<xsl:with-param name="date" select="concat(substring-before(d4,' '),'T',d8,':00')"/>
</xsl:call-template>
</xsl:if>
</xsl:variable>

<xsl:variable name="start_iso8601">
<xsl:call-template name="date2UTC">
<xsl:with-param name="date" select="concat(substring-before(d4,' '),'T',substring-after(d4,' '))"/>
</xsl:call-template>
</xsl:variable> 

<xsl:variable name="stop_iso8601">
<xsl:call-template name="date2UTC">
<xsl:with-param name="date" select="concat(substring-before(d5,' '),'T',substring-after(d5,' '))"/>
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

<xsl:variable name="stop_xmltv">
<xsl:value-of select="concat(translate($stop_iso8601,'-:ZT',''),' +0000')"/>
</xsl:variable>

<xsl:element name="programme">
<xsl:attribute name="start">
<xsl:value-of select="$start_xmltv"/>
</xsl:attribute>
<xsl:attribute name="stop">
<xsl:value-of select="$stop_xmltv"/>
</xsl:attribute>
<xsl:if test="string-length($vps_xmltv)">
<xsl:attribute name="vps-start">
<xsl:value-of select="$vps_xmltv"/>
</xsl:attribute>
</xsl:if>
<xsl:attribute name="channel">
<xsl:value-of select="$channelid"/>
</xsl:attribute>
<xsl:text>&#x0A;</xsl:text>

<xsl:if test="string-length($EVENTID)">
<xsl:comment> pid = <xsl:value-of select="$EVENTID"/><xsl:text> </xsl:text></xsl:comment><xsl:text>&#x0A;</xsl:text>
</xsl:if>

<title lang="de"><xsl:value-of select="d19"/></title><xsl:text>&#x0A;</xsl:text>
<xsl:if test="string-length(d20)">
<sub-title lang="de"><xsl:value-of select="d20"/></sub-title><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length($THEMEN)">
<desc lang="de"><xsl:value-of select="$THEMEN"/></desc><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length($INHALT)">
<desc lang="de"><xsl:value-of select="$INHALT"/></desc><xsl:text>&#x0A;</xsl:text>
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
<xsl:if test="string-length($GENRE)">
<category lang="de"><xsl:value-of select="$GENRE"/></category><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length($PICS)">
<xsl:copy-of select="$PICS"/>
</xsl:if>
<xsl:if test="string-length($LAND)">
<country><xsl:value-of select="$LAND"/></country><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length($EPISODE)">
<episode-num system='xmltv_ns'>.<xsl:value-of select="$EPISODE"/>.</episode-num><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length($VIDEO)">
<video><xsl:text>&#x0A;</xsl:text><xsl:copy-of select="$VIDEO"/></video><xsl:text>&#x0A;</xsl:text>
</xsl:if>
<xsl:if test="string-length($AUDIO)">
<audio><xsl:text>&#x0A;</xsl:text><stereo><xsl:value-of select="$AUDIO"/></stereo><xsl:text>&#x0A;</xsl:text></audio><xsl:text>&#x0A;</xsl:text>
</xsl:if>

<xsl:if test="string-length($STARRATING)">
<xsl:copy-of select="$STARRATING"/>
</xsl:if>
<xsl:if test="string-length($TIPP)">
<xsl:copy-of select="$TIPP"/>
</xsl:if>

</xsl:element>
<xsl:text>&#x0A;</xsl:text>
</xsl:for-each>
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

<xsl:template name="output-tokens">
    <xsl:param name="list" />
    <xsl:param name="delimiter" />
    <xsl:param name="tag" />
    <xsl:param name="last"/>
    <xsl:variable name="newlist">
        <xsl:choose>
            <xsl:when test="contains($list, $delimiter)"><xsl:value-of select="normalize-space($list)" /></xsl:when>
            <xsl:otherwise><xsl:value-of select="concat(normalize-space($list), $delimiter)"/></xsl:otherwise>
        </xsl:choose>
    </xsl:variable>
    <xsl:variable name="first" select="substring-before($newlist, $delimiter)" />
    <xsl:variable name="remaining" select="substring-after($newlist, $delimiter)" />
    <xsl:if test="$first != $last">
    <xsl:text disable-output-escaping="yes">&lt;</xsl:text><xsl:value-of select="$tag"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    <xsl:value-of select="$first" />
    <xsl:text disable-output-escaping="yes">&lt;/</xsl:text><xsl:value-of select="$tag"/><xsl:text disable-output-escaping="yes">&gt;</xsl:text>
    <xsl:text>&#x0A;</xsl:text>
    </xsl:if>
    <xsl:if test="$remaining">
        <xsl:call-template name="output-tokens">
            <xsl:with-param name="list" select="$remaining" />
            <xsl:with-param name="delimiter"><xsl:value-of select="$delimiter"/></xsl:with-param>
            <xsl:with-param name="tag" select="$tag"/>
            <xsl:with-param name="last" select="$first"/>
        </xsl:call-template>
    </xsl:if>
</xsl:template>



</xsl:stylesheet>
