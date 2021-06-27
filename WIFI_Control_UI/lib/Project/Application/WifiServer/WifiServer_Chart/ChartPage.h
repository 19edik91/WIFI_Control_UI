#ifdef USE_CHARTS
const char CHART_PAGE[] = R"=====(
<HTML>
    <HEAD>
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <link rel="icon" href="data:,">
        <script src="https://code.highcharts.com/highcharts.js"></script>
        <style>
            body
            {
                text-align: center;
                font-family: "Trebuchet MS", Arial;
                margin-left: auto;
                margin-right: auto;
            }
            h2
            {
              font-family: Arial;
              font-size: 2.5rem;
              text-align: center;
            }
        </style>
    </HEAD>
    <BODY onload="javascript:start();">
        <CENTER>
            <h2> Chart interface </h2>        
            
            <div id="chart-voltage" class="container"></div>
            <div id="chart-regulation" class="container"></div>
            <div id="chart-current" class="container"></div>
            
            <FORM method="post" action="/form">
                <TR>
                <TD>
                <button TYPE=SUBMIT VALUE="Home" style="WIDTH:200" name=submit>Home</button>
                </TD>
                </TR>
            </FORM>    
        </CENTER>
    </BODY>
    <script>
    var websock;
    var chartV = new Highcharts.Chart(
    {
        chart:{ renderTo: 'chart-voltage'},
        title:{ text: 'Output voltage'},
        series:
        [{ 
            showInLegend: false,
            data:[100]
        }],
        plotOptions:
        {
            line:
            {
                animation: false,
                dataLabels: {enabled: true}
            },
            series:
            {
                color: '#059e8a'
            }
        },
        xAxis:
        {
            type: 'datetime',
            dateTimeLabelFormats: 
            {
                second: '%H:%M:%S'
            }                
        },
        yAxis:
        {
            title: 
            {
                text: 'Voltage in AD Value'
            }
        },
        credits:
        {
            enabled: false
        }
    });
    
    var chartC = new Highcharts.Chart(
    {
        chart:{ renderTo: 'chart-current'},
        title:{ text: 'Output current'},
        series:
        [{ 
            showInLegend: false,
            data:[null]
        }],
        plotOptions:
        {
            line:
            {
                animation: false,
                dataLabels: {enabled: true}
            },
            series:
            {
                color: '#18009c'
            }
        },
        xAxis:
        {
            type: 'datetime',
            dateTimeLabelFormats: 
            {
                second: '%H:%M:%S'
            }                
        },
        yAxis:
        {
            title: 
            {
                text: 'Current in AD Value'
            }
        },
        credits:
        {
            enabled: false
        }
    });
    
    var chartR = new Highcharts.Chart(
    {
        chart:{ renderTo: 'chart-regulation'},
        title:{ text: 'Output regulation'},
        series:
        [{ 
            showInLegend: false,
            data:[null]
        }],
        plotOptions:
        {
            line:
            {
                animation: false,
                dataLabels: {enabled: true}
            },
        },
        xAxis:
        {
            type: 'datetime',
            dateTimeLabelFormats: 
            {
                second: '%H:%M:%S'
            }                
        },
        yAxis:
        {
            title: 
            {
                text: 'PWM value'
            }
        },
        credits:
        {
            enabled: false
        }
    });
    
    function addData(dateTime, CurrentVal, VoltageVal, RegVal)
    {
        if(chartV.series[0].data.length > 40)
        {
            chartV.series[0].addPoint([dateTime, VoltageVal], true, true, true);
        }
        else
        {
            chartV.series[0].addPoint([dateTime, VoltageVal], true, false, true);
        }
        
        if(chartC.series[0].data.length > 40)
        {
            chartC.series[0].addPoint([dateTime, CurrentVal], true, true, true);
        }
        else
        {
            chartC.series[0].addPoint([dateTime, CurrentVal], true, false, true);
        }
        
        if(chartR.series[0].data.length > 40)
        {
            chartR.series[0].addPoint([dateTime, RegVal], true, true, true);
        }
        else
        {
            chartR.series[0].addPoint([dateTime, RegVal], true, false, true);
        }
    }
    
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
                var VoltageVal = evt.data.substring( evt.data.indexOf("V")+1, evt.data.indexOf("v"));
                var CurrentVal = evt.data.substring( evt.data.indexOf("C")+1, evt.data.indexOf("c"));
                var RegVal = evt.data.substring( evt.data.indexOf("R")+1, evt.data.indexOf("r"));
                var dateTime = (new Date()).getTime();
                
				console.log('voltage:');
				console.log(VoltageVal);

                addData(dateTime, CurrentVal, VoltageVal, RegVal);                
            }
        }
    }
</script>
</HTML>
)=====";

#endif //USE_CHARTS