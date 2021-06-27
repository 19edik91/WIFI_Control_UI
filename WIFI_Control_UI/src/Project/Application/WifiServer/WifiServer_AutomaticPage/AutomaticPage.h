const char AUTOMATIC_page[] = R"=====(
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
			function sliderChanged(val)
			{
				<!-- var sliderDiv = document.getElementById("sliderAmount"); -->
				<!-- sliderDiv.innerHTML = val; -->
				
				postSliderValue(val)
			}
			
			function postSliderValue(SliderVal)
			{
				var form = document.createElement("form");
				form.setAttribute("method", "post");
				form.setAttribute("action", "/slider");
       				
				var hiddenField = document.createElement("input");
				hiddenField.setAttribute("type", "hidden");
				hiddenField.setAttribute("name", "SliderValue");
				hiddenField.setAttribute("value", SliderVal);
				form.appendChild(hiddenField);
        
				document.body.appendChild(form);
				form.submit();
			}
		</script>
    
    <TITLE>
      LED remote control
    </TITLE>
  </HEAD>
  <BODY>
    <CENTER>
      <h1> Automatic Motion detection Menu </h1>     
        <FORM method="post" action="/form">
          <TABLE>       
            <TD>Timer:    </TD>     
            <TD> TimerIdx </TD>
            <TD> von </TD>
            <TD> 10 </TD>           
          </TABLE>
          <p> </p>
        
          Startzeit
          <input name="startTimeHours" TYPE=text VALUE="ValStartTimeHours" style="WIDTH:25"></input>
          
          <input name="startTimeMinutes" TYPE=text VALUE="ValStartTimeMin" style="WIDTH:25"></input>          
          <p> </p>
  
          Stopzeit
          <input name="stopTimeHours" TYPE=text VALUE="ValStopTimeHours" style="WIDTH:25"></input>
          
          <input name="stopTimeMinutes" TYPE=text VALUE="ValStopTimeMin" style="WIDTH:25"></input>          
          <p> </p>
          
          
          <button TYPE=SUBMIT VALUE="previousBtn" name=submit> < </button>
          <button TYPE=SUBMIT VALUE="nextBtn" name=submit> > </button>
          <p> </p>

          <button TYPE=SUBMIT VALUE="TimeSet" name=submit>Uhrzeit einstellen</button>          
          <p> </p>

        <table>
          <TR><TD><p> </p></TD></TR>

					<TR>
					<TD>0 min</TD>
					<TD> </TD>
					<TD>60 min</TD>
					</TR>
         
					<TR>
					<TD><input id="slide" class="slider" type="range" min="0" max="60" step="5" value=@@SLIDERVAL@@ onchange="sliderChanged(this.value)"></TD>				        
          </TR>
          <TR>
          <TD>Beleuchtungsdauer einstellen:</TD>
          <TD>@@L3@@</TD>
          <TD> min </TD>
          </TR>

          <TR><TD><p> </p></TD></TR>
        </table>

        <table>
        <TR>
          <TD> Nachtmodus: </TD>
          <TD><button TYPE=SUBMIT VALUE="NightON" name=submit>EIN</button></TD>
          <TD><button TYPE=SUBMIT VALUE="NightOFF" name=submit>AUS</button></TD>
          <TD>@@A1@@</TD>
          <p> </p>
        </TR>

        <TR>
          <TD>Bewegungsmelder:</TD>
          <TD><button TYPE=SUBMIT VALUE="PirON" name=submit>EIN</button></TD>
          <TD><button TYPE=SUBMIT VALUE="PirOFF" name=submit>AUS</button></TD>
          <TD>@@B1@@</TD>
          <p> </p>
        </TR>

        <TR>
          <TD>Automatik Modus:</TD>
          <TD><button TYPE=SUBMIT VALUE="AutoON" name=submit>EIN</button></TD>
          <TD><button TYPE=SUBMIT VALUE="AutoOFF" name=submit>AUS</button></TD>
          <TD>@@C1@@</TD>
          <p> </p>
        </TR>

        </table>
          <button TYPE=SUBMIT VALUE="HomeBtnAuto" style="WIDTH:200" name=submit>Home</button>
        </FORM>
    </CENTER>
  </BODY>
</HTML>
)=====";
