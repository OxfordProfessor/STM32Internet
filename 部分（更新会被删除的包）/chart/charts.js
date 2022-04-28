export default {
    data() {
      return {
        //trend图
        trendChart: {
          grid: {
              left: 'left',
              top: 50,
              containLabel: true,
              tooltip: {
                triggerOn: 'none',
                showConent: true,
                position: function (pt) {
                  return [pt[0], pt[1]-50];
                }
              }
          },
          tooltip: {
            trigger: "none",
            showContent: false,
          },
          textStyle: {
            color: "#999",
            fontSize: 24
          },
          label: {
            fontSize: 22
          },
          xAxis: {
            name: "年份",
            type: "category",
            nameGap:10, //坐标轴名称与轴线之间的距离。
            boundaryGap: true, //坐标轴两边留白策略
            nameTextStyle:{ //坐标轴名称样式
                color:"#999",
                fontSize: 12,
                align: 'left',
                verticalAlign: 'bottom'
            },
            axisLine: { //坐标轴轴线相关设置
              show: true, //是否显示坐标轴轴线。
              symbol: ['none','arrow'], //轴线两边的箭头默认不显示箭头，即 'none'。两端都显示箭头可以设置为 'arrow'，只在末端显示箭头可以设置为 ['none', 'arrow']。
              symbolSize: [10,8],//轴线两边的箭头的大小
              symbolOffset: [0,5],//轴线两边的箭头的偏移
              lineStyle: {
                color: "#ece9e2",//线条颜色
              },
            },
            axisTick: { //坐标轴刻度相关设置
              show: false
            },
            axisLabel: { //坐标轴刻度标签的相关设置
              interval: 10000,
              showMinLabel: true,
              showMaxLabel: true,
              fontSize: 12,
              padding: [6, 0, 0, 0]
            },
            axisPointer: {  //坐标轴指示器配置项
                value: '',
                snap: true,
                type: 'line', //指示器类型
                show: false, //竖线是否显示，作用于每一个点
                lineStyle: {
                    color: '#ece9e2',
                    width: 1
                },
                label: { //坐标轴指示器的文本标签
                    show: false,
                },
                handle: { //拖拽手柄，适用于触屏的环境
                    show: true,
                    color: 'none'
                }
            },
            data: []
          },
          yAxis: {
            type: "value",
            name: "价格(元)",
            nameGap: 0,
            nameTextStyle:{
                color:"#999",
                fontSize: 12,
                align: 'right',
                verticalAlign: 'top',
                padding: [0,0,10,60]
            },
            axisLine: {
              show: true,
              length: 100,
              symbol: ['none','arrow'],
              symbolSize: [10,8],
              symbolOffset: [0,5],
              lineStyle: {
                color: "#ece9e2",
              },
  
            },
            axisLabel: {
              fontSize: 12,
              formatter: value => {
                return value;
              }
            },
            axisTick: {
              show: false
            },
            splitLine:{
                lineStyle: {
                    //网络线设置（只作用于非类目铀）
                    show: true,
                    color: "#ece9e2",
                    width: 0.5,
                    type: "solid"
                },
            },
            splitNumber: 5,
            min: 0,
            max: 4000,
            interval: 1000
          },
          series: [
            {
              type: "line",
              smooth: false,
              color: "#ca3c2e",
              showSymbol: true,
              lineStyle: {
                width: 1.5,
                color: "#c5936e",
              },
              itemStyle: {
                  normal:{
                      borderWidth: 0.5,
                      label:{
                          show: true, //显示值
                          borderWidth: 2,
                          color: '#c5936e',
                          fontSize: 12,
                      }
                  }
              },
              data: []
            },
          ]
        },
      };
    },
    methods: {
        initCombineLineData(data) {
            this.trendChart.xAxis.axisPointer.value = data[data.length-1].date; //让指示器定位在最后一个折线点上
            for(let i=0;i<=data.length;i++){
                let yData = {
                    symbol: 'none'  //折线上不显示转折点
                };
                if(i== data.length-1){
                    yData.symbol = "emptyCircle", //最后一个显示转折点
                    yData.symbolSize = 6
                }
                yData.value = data[i].price;
  
                this.trendChart.xAxis.data.push(data[i].date);
                this.trendChart.series[0].data.push(yData);
  
            }
        },
    }
  };