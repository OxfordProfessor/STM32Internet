var mqtt = require('aliyun-mqtt/mqtt.min.js') //根据自己存放的路径修改
const crypto = require('aliyun-mqtt/hex_hmac_sha1.js'); //根据自己存放的路径修改

function InitServer()
{
  var that = this
  var model = this.storage
  var model_voltage = this.voltage
  //注意：这里在程序运行后会直接进行连接，如果你要真机调试，记得关掉模拟器或者使用一个按钮来控制连接，以避免模拟器和真机同时进行连接导致两边都频繁断线重连！
  const deviceConfig = {
    productKey: "gvrxJiLWkq4",
    deviceName: "Stm32Internet",
    deviceSecret: "3758872a01820a61a32d61bc9d3b845e",
    regionId: "cn-shanghai"//根据自己的区域替换
  };
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
}

