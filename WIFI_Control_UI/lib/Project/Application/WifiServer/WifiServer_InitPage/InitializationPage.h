const char INIT_PAGE[] = R"=====(
<HTML>
	<HEAD>
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<link rel="icon" href="data:,">
		<style>
			body
			{
				text-align: center;
				font-family: "Trebuchet MS", Arial;
				margin-left: auto;
				margin-right: auto;
			}
			.slider {
			  -webkit-appearance: none;
			  width: 135%;
			  height: 15px;
			  border-radius: 5px;   
			  background: #d3d3d3;
			  outline: none;
			  opacity: 0.7;
			  -webkit-transition: .2s;
			  transition: opacity .2s;
			}

			.slider::-webkit-slider-thumb {
			  -webkit-appearance: none;
			  appearance: none;
			  width: 25px;
			  height: 25px;
			  border-radius: 50%; 
			  background: #4CAF50;
			  cursor: pointer;
			}

			.slider::-moz-range-thumb {
			  width: 25px;
			  height: 25px;
			  border-radius: 50%;
			  background: #4CAF50;
			  cursor: pointer;
			}
		</style>
		
		<script>
            var websock;
            function start()
            {
                websock = new WebSocket('ws://' + window.location.hostname + ':81/');
                websock.onopen = function(evt){ console.log('websock open');};
                websock.onclose = function(evt){ console.log('websock close');};
                websock.onerror = function(evt){ console.log(evt);};

                websock.onmessage = function(evt)
                {
                    if(evt.data.substring(0,1) == "V")
                    {
						document.getElementById('VoltId').innerHTML = evt.data.substring( evt.data.indexOf("V")+1, evt.data.indexOf("v"));
						document.getElementById('CurrentId').innerHTML = evt.data.substring( evt.data.indexOf("C")+1, evt.data.indexOf("c"));
                        document.getElementById('TempId').innerHTML = evt.data.substring( evt.data.indexOf("K")+1, evt.data.indexOf("k"));
                    }
                    else if(evt.data.substring(0,1) == "B")
                    {
                        document.getElementById('slide').value = evt.data.substring( evt.data.indexOf("B")+1, evt.data.indexOf("b"));
                        document.getElementById('BrightId').innerHTML = evt.data.substring( evt.data.indexOf("B")+1, evt.data.indexOf("b"));
                    }
					else if(evt.data.substring(0,1) == "O")
					{
						document.getElementById('OutIdx').innerHTML = evt.data.substring( evt.data.indexOf("O")+1, evt.data.indexOf("o"));
						document.getElementById('OutCnt').innerHTML = evt.data.substring( evt.data.indexOf("N")+1, evt.data.indexOf("n"));
						document.getElementById('MinBrightId').innerHTML = evt.data.substring( evt.data.indexOf("L")+1, evt.data.indexOf("l"));
						document.getElementById('MaxBrightId').innerHTML = evt.data.substring( evt.data.indexOf("U")+1, evt.data.indexOf("u"));
						document.getElementById('AISTATE').innerHTML = evt.data.substring( evt.data.indexOf("A")+1, evt.data.indexOf("a"));
						document.getElementById('MISTATE').innerHTML = evt.data.substring( evt.data.indexOf("M")+1, evt.data.indexOf("m"));
						document.getElementById('slide').value = evt.data.substring( evt.data.indexOf("B")+1, evt.data.indexOf("b"));
                        document.getElementById('BrightId').innerHTML = evt.data.substring( evt.data.indexOf("B")+1, evt.data.indexOf("b"));
					}
                    else
                    {
                        console.log('unknown event');
                    }
                }
            }

            function sliderChanged(val)
            {
                websock.send("B"+val+"b");
                var sl = "Slider: " + "B"+val+"b";
                console.log(sl);
            }
            
            function buttonClicked(index)
            {
                websock.send("L"+index+"l");
                var bt = "Button: " + "L"+index+"l";
                console.log(bt);
            }
		</script>
		
		<TITLE>
			LED remote control
		</TITLE>
	</HEAD>
	<BODY onload="javascript:start();">
		<CENTER>
			<h1> Initialisierungs Menu </h1>
			
				<FORM>
					<TABLE>
						<TR>
						<TD>Automatische Initialisierung</TD>
						</TR>
						
						<TR>
							<TD>
							<button type="button" value="AION" onclick=buttonClicked(this.value)>START</button>
							<button type="button" value="AIOFF" onclick=buttonClicked(this.value)>STOP</button>
							</TD>
							<TD><p id='AISTATE'>@@AISTATE@@</p></TD>
						</TR>

						<TR>
						<TD>Manuelle Initialisierung </TD>
						</TR>
						<TR>						
							<TD>
							<button type="button" value="MION" onclick=buttonClicked(this.value)>START</button>
							<button type="button" value="MIOFF" onclick=buttonClicked(this.value)>STOP</button>
							</TD>
							<TD><p id='MISTATE'>@@MISTATE@@</p></TD>						
						</TR>
					</TABLE>				
				
					<TABLE>
						<TR><h3>Helligkeit einstellen:</h3></TR>		
						<TR>
						<button type="button" value="Prev" onclick=buttonClicked(this.value)> < </button>
						<button type="button" value="Next" onclick=buttonClicked(this.value)> > </button>	
						</TR>						
						<TR>
						<TD>Output:</TD>
						<TD><p id='OutIdx'>@@OutputIdx@@</p></TD>
						<TD> von </TD>
						<TD><p id='OutCnt'>@@OutputCnt@@</p></TD>
						</TR>
					</TABLE>
					<TABLE>
						<TR>
						<TD>5%</TD>
						<TD> </TD>
						<TD>100%</TD>
						</TR>
							
						<TR>
							<TD style="width:55%"><input id="slide" class="slider" type="range" min="5" max="100" step="5" value=@@SLIDERVAL@@ onchange="sliderChanged(this.value)"></input></TD>
						</TR>
						<TR>
							<TD>Aktuelle Helligkeit </TD>
							<TD><p id='BrightId'>@@BR@@</p></TD>
							<TD> % </TD>
						</TR>
						<TR>
							<TD>Minimale Helligkeit </TD>
							<TD><p id='MinBrightId'>@@MinBR@@</p></TD>
							<TD> % </TD>
						</TR>
						<TR>
							<TD>Maximale Helligkeit </TD>
							<TD><p id='MaxBrightId'>@@MaxBR@@</p></TD>
							<TD> % </TD>
						</TR>
						<TR>
							<TD> Temperatur </TD>
							<TD>
							<p id='TempId'>@@T1@@</p>
							</TD>
							<TD> C </TD>
						</TR>
						<TR>
							<TD> Spannung </TD>
							<TD>
							<p id='VoltId'>@@V@@</p>
							</TD>
							<TD> mV</TD>
						</TR>
						<TR>
							<TD> Strom </TD>
							<TD>
							<p id='CurrentId'>@@A@@</p>
							</TD>
							<TD> mA</TD>
						</TR>
						<TR>
							<TD><button type="button" value="MinSet" onclick=buttonClicked(this.value)>Set MIN</button></TD>
							<TD><p></p></TD>
							<TD><button type="button" value="MaxSet" onclick=buttonClicked(this.value)>Set MAX</button></TD>
							</TD>
						</TR>
					</TABLE>
				</FORM>
				<FORM method="post" action="/form">
				  <TD>
					<button TYPE=SUBMIT VALUE="HomeBtn" style="WIDTH:200" name=submit>Home menu</button>              
				  </TD>
				</FORM>
			</CENTER>
	</BODY>
</HTML>
)=====";
