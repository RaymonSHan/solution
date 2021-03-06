
		var option = "";


		function setOption(opts) {
			alert("aa");
			option = op;
	        }

alert("in sub");
		var rwidth = 1000;
		var rheight = 600;
		var rtop = 50;
		var rbottom = 50;
		var rleft = 50;
		var rright = 50;
		var rinterper = 0.1;
		var routerper = 0.3;
		var rcolor = ["red", "blue", "green"];

		var dataSet = [];
		var barType = option.series.length;
		var barNum = option.xAxis[0].data.length;
		var barGroupWidth = (rwidth - rleft - rright) / barNum;
		var barWidth = barGroupWidth / (barType * (1 + rinterper) + (routerper - rinterper));

		var rdata = option.series[0].data;
		for (var i = 1; i < barType; i++) {
			rdata.push.apply(rdata, option.series[i].data);
		}
		var rMax = d3.max(rdata);
		var rMin = d3.min(rdata);
		var sMax = (rMax < 0) ? 0 : rMax;
		var sMin = (rMin > 0) ? 0 : rMin;

		var svg = d3.select("body").append("svg")
								.attr("width", rwidth)
								.attr("height", rheight);

		var xScale = d3.scale.ordinal()
						.domain(d3.range(barNum))
						.rangeRoundBands([0, rwidth - rleft - rright]);

		var yScale = d3.scale.linear()
						.domain([sMin, sMax])
						.range([rheight - rtop - rbottom, 0]);



		var xAxis = d3.svg.axis()
						.scale(xScale)
						.orient("bottom")
						.tickFormat(function(d, i) {
				            return option.xAxis[0].data[i];
				        });
		
		var yAxis = d3.svg.axis()
						.scale(yScale)
						.orient("left");

		svg.selectAll("rect")
		   .data(rdata)
		   .enter()
		   .append("rect")
		   .attr("x", function(d,i){
				var rmod = i % barNum;
				var rdiv = (i - rmod) / barNum;
				var rxp = barGroupWidth * rmod + (rdiv * (1 + rinterper) + routerper / 2) * barWidth;
				return rleft + rxp;
		   })
		   .attr("width", function(d,i){
				return barWidth;
		   })
          .on("mouseover",function(d,i){  
                d3.select(this)  
                  .attr("fill","yellow"); 
		   })
           .on("mouseout",function(d,i){  
                d3.select(this)  
                  .transition()  
                  .duration(500)  
                  .attr("fill",function(){
				  	var rmod = i % barNum;
					var rdiv = (i - rmod) / barNum;
					rdiv = rdiv % rcolor.length;
					return rcolor[rdiv];
		   		})
		   })

		   .attr("height",0)
		   .attr("y",function(d,i){
				return rheight - rbottom;
		   })
 		   .attr("fill", "yellow")
		   .transition()
		   .ease("circle")
		   .duration(3000)
		   .delay(function(d,i){
				var rmod = i % barNum;
				var rdiv = (i - rmod) / barNum;
				rdiv = rdiv % rcolor.length;
				return 200 * rmod;
		   })

		   .attr("y",function(d,i){
				return rtop + yScale(d);
		   })
		   .attr("height", function(d,i){
				return  rheight - rtop - rbottom - yScale(d);
		   })
		   .attr("fill",function(d,i){
				var rmod = i % barNum;
				var rdiv = (i - rmod) / barNum;
				rdiv = rdiv % rcolor.length;
				return rcolor[rdiv];
		   }) ;

	   
		svg.selectAll("text")
            .data(rdata)
            .enter().append("text")
		    .attr("x", function(d,i){
				var rmod = i % barNum;
				var rdiv = (i - rmod) / barNum;
				var rxp = barGroupWidth * rmod + (rdiv * (1 + rinterper) + routerper / 2 + 1/2) * barWidth
				return rleft + rxp;
		    })
		    .attr("y",function(d,i){
				return rtop - 5 + yScale(d);
		    })
			.attr("text-anchor", "middle")
			.attr("font-size", 14) 
			.attr("fill",function(d,i){
				var rmod = i % barNum;
				var rdiv = (i - rmod) / barNum;
				rdiv = rdiv % rcolor.length;
				return rcolor[rdiv];
		    })
			.text("")
		    .transition()
		    .delay(barNum * 200 + 3000)

            .text(function(d,i){
				return d;
			});
		   
		svg.append("g")
			.attr("class","axis")
			.attr("transform","translate(" + rleft + "," + (rheight - rbottom) + ")")
			.call(xAxis);
			
		svg.append("g")
			.attr("class","axis")
			.attr("transform","translate(" + rleft + "," + rtop + ")")
			.call(yAxis); 
		
