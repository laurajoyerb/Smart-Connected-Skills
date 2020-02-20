var app = require('express')();
var http = require('http').Server(app);

function toogleDataSeries(e){
	if (typeof(e.dataSeries.visible) === "undefined" || e.dataSeries.visible) {
		e.dataSeries.visible = false;
	} else{
		e.dataSeries.visible = true;
	}
	chart.render();
}

var amazon = [];
var facebook = [];
var google = [];
var microsoft = [];

var chartOptions = {
	title: {
		text: "Stocks"
	},
	axisX: {
    interval: 1
	},
	axisY2: {
		title: "Closing Value",
		prefix: "$",
	},
	toolTip: {
		shared: true
	},
	legend: {
		cursor: "pointer",
		verticalAlign: "top",
		horizontalAlign: "center",
		dockInsidePlotArea: true,
		itemclick: toogleDataSeries
	},
	data: [{
		type:"line",
		axisYType: "secondary",
		name: "AMZN",
		showInLegend: true,
		markerSize: 0,
		yValueFormatString: "$#,###",
		dataPoints: amazon
	},
	{
		type: "line",
		axisYType: "secondary",
		name: "FB",
		showInLegend: true,
		markerSize: 0,
		yValueFormatString: "$#,###",
		dataPoints: facebook
	},
	{
		type: "line",
		axisYType: "secondary",
		name: "GOOGL",
		showInLegend: true,
		markerSize: 0,
		yValueFormatString: "$#,###",
		dataPoints: google
	},
	{
		type: "line",
		axisYType: "secondary",
		name: "MSFT",
		showInLegend: true,
		markerSize: 0,
		yValueFormatString: "$#,###",
		dataPoints: microsoft
	}]
};

var io = require('socket.io')(http);

app.get('/', function(req, res){
  res.sendFile(__dirname + '/index.html');
  app.get('/data', function(req, res) {
    res.sendFile(__dirname + '/stocks.csv');
  });
});

var fs = require('fs')
  , filename = __dirname + '/stocks.csv';
var csv_lines = fs.readFileSync(filename, 'utf8').toString().split("\n");
var data = [];
for (i=1;i<csv_lines.length; i++) {
	data = csv_lines[i].split(",");
	switch (data[1]) {
		case 'AMZN':
			amazon.push({
				x: parseInt(data[0], 10),
				y: parseInt(data[2], 10)
			});
			break;
		case 'FB':
			facebook.push({
				x: parseInt(data[0], 10),
				y: parseInt(data[2], 10)
			});
			break;
		case 'GOOGL':
			google.push({
				x: parseInt(data[0], 10),
				y: parseInt(data[2], 10)
			});
			break;
		case 'MSFT':
			microsoft.push({
				x: parseInt(data[0], 10),
				y: parseInt(data[2], 10)
			});
			break;
		default:
			console.log("Error getting data");
			break;
	}
}

io.on('connection', function(socket){
  io.emit('dataMsg', chartOptions);

  socket.on('disconnect', function(){
    console.log('user disconnected');
  });
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});
