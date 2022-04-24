<template>
  <view class="control">
    <view class="text-voltage">
      <view class="data-title">当前设备剩余电量</view>
      <view class="data-value">{{voltage}}%</view>
    </view>
    <view class="text-storage">
      <view class="data-title">当前设备剩余容量</view>
      <view class="data-value">{{storage}}%</view>      
    </view>
    <view class="Condition-display">
    <view :wx:if="voltage <= 20 || storage <= 20">
      <view class="data-title">警告!</view>
      <view :wx:if="voltage <= 20 ">
        <view class="voltage-information">您的电量即将耗尽，建议您尽快靠岸</view>
      </view>
      <view :wx:if="storage <= 20">
        <view class="storage-information">您的垃圾清理机器人垃圾仓已满，建议您尽快靠岸、并清理垃圾仓</view>
      </view>
    </view>
    </view>
    <button class='btn1' @tap="BankReturn">返回岸边</button>
  </view>
</template>

<script>
var mqtt = require('aliyun-mqtt/mqtt.min.js') //根据自己存放的路径修改
const crypto = require('aliyun-mqtt/hex_hmac_sha1.js'); //根据自己存放的路径修改
import global_ from 'Global/Global'

export default {
  data () {
    return {
      storage: 0,
      voltage: 0,
      direction: 0,              //虚拟摇杆方位角，1表示第一象限，2表示第二象限，3表示第三象限，4表示第四象限，0位于居中位置
      Comeback: 0,               //返回岸边标志位，1表示返回岸边
    }
  },
  components: {

  },
  mounted(){

  },
  methods:{
    BankReturn: function(event) {     //按键触发事件，将全局变量Comebakc置为1
      var that = this
      global_.Comeback = 1
      console.log('更改数值');
    }
  },
  created () {
    
  },
  onLoad: function(options){    //接收页面传参
    var that = this
    that.storage = global_.storage
  },
  created: function(){

  }
}

</script>

<style lang="scss" scoped>
.control{
  .text-voltage{
    padding: 0px 30px;
    margin-top: 15px;
    color: #7f7f7f;
    .data-value{
      padding: 15px 0px;
      font-size: 26px;
      color: #f9a13d;
    }
  }
  .text-storage{
    padding: 0px 30px;
    margin-top: 15px;
    color: #7f7f7f;
    .data-value{
      padding: 15px 0px;
      font-size: 26px;
      color: #3d7ef9;
    }
  }
  .Condition-display{
    padding: 0px 30px;
    margin-top: 26px;
    color: #7f7f7f;
    .voltage-information{
      padding: 15px 0px;
      font-size: 15px;
      color: #eb0b0b;
    }
    .storage-information{
      padding: 15px 0px;
      font-size: 15px;
      color: #f30b0b;
    }
  }
  .btn1{
    width:80%;
    margin-top:130rpx;
    background-color: rgb(106, 0, 95);
    color: white;
    border-radius: 98rpx;
    display: flex;
    flex-direction: row;
    align-items: center;
    justify-content: center;
  }
}
</style>
