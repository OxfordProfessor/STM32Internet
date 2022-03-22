<template>
  <div class="wrapper">
    <div class="header-wrapper">
      <div class="header-title">
        <span>空气质量-良好</span>
        <span>武汉市</span>
      </div>
      <div class="header-text">
        <span>55</span>
        <span>阴</span>
      </div>
      <div class="weather-advice">空气质量良好、健康人群外出活动无需可以刻意防护</div>
    </div>
    <div class="body-wrapper">
      <div class="body">
        <div class="data-wrapper">
          <div class="data">
            <img class="data-logo" src="/static/images/PH.png"/>
            <div class="data-text">
              <div class="data-title">PH值</div>
              <div class="data-value">6.8</div>
            </div>
          </div>
          <div class="data-TDS">
            <img class="data-logo" src="/static/images/TDS.png"/>
            <div class="data-text">
              <div class="data-title">TDS值</div>
              <div class="data-value">508mg/L</div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>

var mqtt = require('aliyun-mqtt/mqtt.min.js') //根据自己存放的路径修改
const crypto = require('aliyun-mqtt/hex_hmac_sha1.js'); //根据自己存放的路径修改

//export default {
Page({
  data() {

  
  },
  onLoad(){
  	//注意：这里在程序运行后会直接进行连接，如果你要真机调试，记得关掉模拟器或者使用一个按钮来控制连接，以避免模拟器和真机同时进行连接导致两边都频繁断线重连！
    this.doConnect();
  },
  doConnect(){
    const deviceConfig = {
      productKey: "gvrxJiLWkq4",
      deviceName: "Stm32Internet",
      deviceSecret: "3758872a01820a61a32d61bc9d3b845e",
      regionId: "cn-shanghai"//根据自己的区域替换
    };
    const options = this.initMqttOptions(deviceConfig);
    console.log(options)
    //替换productKey为你自己的产品的（注意这里是wxs，不是wss，否则你可能会碰到ws不是构造函数的错误）
    const client = mqtt.connect('wxs://gvrxJiLWkq4.iot-as-mqtt.cn-shanghai.aliyuncs.com',options)
    client.on('connect', function () {
      console.log('连接服务器成功')
      //注意：订阅主题，替换productKey和deviceName(这里的主题可能会不一样，具体请查看控制台-产品详情-Topic 类列表下的可订阅主题)，并且确保改主题的权限设置为可订阅
      client.subscribe('/gvrxJiLWkq4/Stm32Internet/user/get', function (err) {
        if (!err) {
           console.log('订阅成功！');
        }
      })
    })
	//接收消息监听
    client.on('message', function (topic, message) {
      // message is Buffer
      let msg = message.toString();
      console.log('收到消息：'+msg);
     //关闭连接 client.end()
    })
  },
  initMqttOptions(deviceConfig) {

    const params = {
      productKey: deviceConfig.productKey,
      deviceName: deviceConfig.deviceName,
      timestamp: Date.now(),
      clientId: Math.random().toString(36).substr(2),
    }
    //CONNECT参数
    const options = {
      keepalive: 60, //60s
      clean: true, //cleanSession不保持持久会话
      protocolVersion: 4 //MQTT v3.1.1
    }
    //1.生成clientId，username，password
    options.password = `5ea42da99af0edcf2f32c8a4db389abbad3596dc`;
    options.clientId = `FESA234FBDS24|securemode=3,signmethod=hmacsha1,timestamp=789|`;
    options.username = `Stm32Internet&gvrxJiLWkq4`;

    return options;
  },

/*
  生成基于HmacSha1的password
  参考文档：https://help.aliyun.com/document_detail/73742.html?#h2-url-1
*/
  signHmacSha1(params, deviceSecret) {

    let keys = Object.keys(params).sort();
    // 按字典序排序
    keys = keys.sort();
    const list = [];
    keys.map((key) => {
      list.push(`${key}${params[key]}`);
    });
    const contentStr = list.join('');
    return crypto.hex_hmac_sha1(deviceSecret, contentStr);
  }
})
//}
</script>

<style lang="scss" scoped>
.wrapper{
  padding: 15px;
  .header-wrapper{
    background-color: #3d7ef9;
    border-radius: 20px;
    color: #fff;
    box-shadow: #d6d6d6 0px 0px 5px;
    padding: 15px 30px;
    .header-title{
      display: flex;
      justify-content: space-between;
    }
    .header-text{
      font-size: 32px;
      font-weight: 400;
      display: flex;
      justify-content: space-between;
    }
    .weather-advice{
      margin-top: 20px;
      font-size: 12px;
    }
  }
  .data-wrapper{
    margin-top: 20px;
    display: flex;
    justify-content: space-between;
    .data{
      background-color: #fff;
      width: 150px;
      height: 80px;
      border-radius: 20px;
      display: flex;
      justify-content: space-around;
      padding: 0 8px;
      box-shadow: #d6d6d6 0px 0px 5px;
      .data-logo{
        height: 60px;
        width: 60px;
        margin-top: 15px;
      }
      .data-text{
        margin-top: 15px;
        color: #7f7f7f;
        .data-value{
          font-size: 26px;
        }
      }
    }
    .data-TDS{
      background-color: #fff;
      width: 150px;
      height: 80px;
      border-radius: 20px;
      display: flex;
      justify-content: space-around;
      padding: 0 8px;
      box-shadow: #d6d6d6 0px 0px 5px;
      .data-logo{
        height: 48px;
        width: 48px;
        margin-top: 20px;
      }
      .data-text{
        margin-top: 15px;
        color: #7f7f7f;
        .data-value{
          font-size: 20px;
        }
      }
    }
  }
}
</style>
