const char ROOT_PAGE[] PROGMEM = R"=====(
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
              width: 100%;
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
                    if(evt.data.substring(0,1) == "K")
                    {
                        document.getElementById('TempId').innerHTML = evt.data.substring( evt.data.indexOf("K")+1, evt.data.indexOf("k"));
                    }
                    else if(evt.data.substring(0,1) == "B")
                    {
                        var Index = evt.data.substring(evt.data.indexOf("I")+1, evt.data.indexOf("i"));
                        document.getElementById('slide'+Index).value = evt.data.substring( evt.data.indexOf("B")+1, evt.data.indexOf("b"));
                        document.getElementById('BrightId'+Index).innerHTML = evt.data.substring( evt.data.indexOf("B")+1, evt.data.indexOf("b"));
                    }
                    else if(evt.data.substring(0,1) == "H")
                    {
                        var hours = evt.data.substring( evt.data.indexOf("H")+1, evt.data.indexOf("h"));
                        var minutes = evt.data.substring( evt.data.indexOf("M")+1, evt.data.indexOf("m"))
                        document.getElementById('Clock').innerHTML = hours + ":" + minutes; 
                    }
                    else if(evt.data.substring(0,1) == "T")
                    {
                        document.getElementById('RemBurn').innerHTML = evt.data.substring( evt.data.indexOf("T")+1, evt.data.indexOf("t"));
                        document.getElementById('BurnTime').innerHTML = evt.data.substring( evt.data.indexOf("O")+1, evt.data.indexOf("o"));
                    }
                    else if(evt.data.substring(0,1) == "L")
                    {
                        var Index = evt.data.substring(evt.data.indexOf("L")+1, evt.data.indexOf("l"));
                        var State = evt.data.substring( evt.data.indexOf("S")+1, evt.data.indexOf("s"));

                        if(State == 0)
                        {
                            document.getElementById('StateId'+Index).innerHTML = 'OFF';
                        }
                        else
                        {
                            document.getElementById('StateId'+Index).innerHTML = 'ON';
                        }
                    }
                    else
                    {
                        console.log('unknown event');
                    }
                }
            }

            function sliderChanged(val, index)
            {
                websock.send("B"+val+"b"+"I"+index+"i");
                var sl = "Slider: " + "B"+val+"b"+"I"+index+"i";
                console.log(sl);
            }
            
            function buttonClicked(index, val)
            {
                websock.send("L"+index+"l"+"S"+val+"s");
                var bt = "Button: " + "L"+index+"l"+"S"+val+"s";
                console.log(bt);
            }
        </script>
        
        <TITLE>
            LED remote control
        </TITLE>
    </HEAD>
    <BODY onload="javascript:start();">
        <CENTER>
            <h1> IoT LED control </h1>        
            
                <FORM method="post" action="/form">
                    <TABLE>
                        <TR>
                        <TD><p>Uhrzeit: </p></TD>
                        <TD><p id="Clock">@@Clock@@</p></TD>
                        </TR>

                        <TR>
                        <TD><p>Restbrenndauer: </p></TD>
                        <TD><p id="RemBurn">@@RemBurn@@</p></TD>
                        <TD>von</TD>
                        <TD><p id="BurnTime">@@BurnTime@@</p></TD>
                        </TR>
                        
                        <TR>
                        <TD> Temperatur </TD>
                        <TD>
                        <p id='TempId'>@@T1@@</p>
                        </TD>
                        <TD> C </TD>
                        </TR>

                        <TR>
                        <TD><p> </p></TD>
                        </TR>
                    </TABLE>
                    <TR>
                    <TD>
                    <button TYPE=SUBMIT VALUE="InitMenu" style="WIDTH:200" name=submit>Initialisierungs Menu</button>
                    </TD>
                    </TR>
                    
                    <TR>
                    <TD>
                    <button TYPE=SUBMIT VALUE="AutoMenu" style="WIDTH:200" name=submit>Automatik Menu</button>
                    </TD>
                    </TR>
                                        
                    <TR>
                    <TD>
                    <button TYPE=SUBMIT VALUE="ChartMenu" style="WIDTH:200" name=submit>Charts</button>
                    </TD>
                    </TR>
                </FORM>
                
                <TABLE>
                    <TR>
                        <TD><p>LED 1</p></TD>
                        <TD style="width:50%"><input id="slide1" class="slider" type="range" min="5" max="100" step="5" value=@@SLIDERVAL1@@ onchange="sliderChanged(this.value,1)"></input></TD>
                        <TD><p id='BrightId1'>@@BR1@@</p></TD>
                        <TD> % </TD>
                        <TD>
						<button type="button" value="1" onclick=buttonClicked(this.value,1)>ON</button>
                        <button type="button" value="1" onclick=buttonClicked(this.value,0)>OFF</button>
						</TD>
                        <TD><p id='StateId1'>@@L1@@</p></TD>                        
                    </TR>
                    <TR>
                        <TD><p>LED 2</p></TD>
                        <TD style="width:50%"><input id="slide2" class="slider" type="range" min="5" max="100" step="5" value=@@SLIDERVAL2@@ onchange="sliderChanged(this.value,2)"></input></TD>
                        <TD>
                        <p id='BrightId2'>@@BR2@@</p>
                        </TD>
                        <TD> % </TD>
                        <TD>
                        <button type="button" value="2" onclick=buttonClicked(this.value,1)>ON</button>
                        <button type="button" value="2" onclick=buttonClicked(this.value,0)>OFF</button>
                        </TD>
                        <TD><p id='StateId2'>@@L2@@</p></TD>                         
                    </TR>
                    <TR>
                        <TD><p>LED 3</TD>
                        <TD style="width:50%"><input id="slide3" class="slider" type="range" min="5" max="100" step="5" value=@@SLIDERVAL3@@ onchange="sliderChanged(this.value,3)"></input></TD>
                        <TD>
                        <p id='BrightId3'>@@BR3@@</p>
                        </TD>
                        <TD> % </TD>
                        <TD>
                        <button type="button" value="3" onclick=buttonClicked(this.value,1)>ON</button>
                        <button type="button" value="3" onclick=buttonClicked(this.value,0)>OFF</button>
                        </TD>
                        <TD><p id='StateId3'>@@L3@@</p></TD>                         
                    </TR>
                </TABLE>        
        </CENTER>
    </BODY>
</HTML>
)=====";
