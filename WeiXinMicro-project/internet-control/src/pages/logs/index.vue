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
    <view class='title-control'>人为辅助控制</view>
    <view @touchmove.stop.prevent="onBanScroll"> 
      <view class='roter1'>
        <view class="rj-joystick-container" id="rj-joystick" :style="{ width: outerRadius * 2 + 'px', height: outerRadius * 2 + 'px'}">
          <view class="outer-view" :style="{ width: outerRadius * 2 + 'px', height: outerRadius * 2 + 'px'}">
            <text v-for="n in 4" :key="n" class="control-direction cuIcon-right lg text-gray">></text>
              <view class="inner-view" :class="{ 'un-move': !isMoving }" :style="{ width: innerRadius * 2 + 'px', height: innerRadius * 2 + 'px', left: innerLeft + 'px', top: innerTop + 'px'}"
                @touchmove="onJoystickMove" @touchend="joystickRestore" @touchstart="onMoveStart">
              </view>
            </view>
          </view>
      </view>
    </view>
  </view>
</template>

<script>
var mqtt = require('aliyun-mqtt/mqtt.min.js') //根据自己存放的路径修改
const crypto = require('aliyun-mqtt/hex_hmac_sha1.js'); //根据自己存放的路径修改
import global_ from 'Global/Global'

export default {
  props: {
      // 操纵杆外圈半径
      outerRadius: {
        type: Number,
        default: 75
      },
      // 操纵杆内圈半径
      innerRadius: {
        type: Number,
        default: 37
      }
  },
  data () {
    return {
      storage: 0,
      voltage: 0,
      //direction: 0,              //虚拟摇杆方位角，1表示第一象限，2表示第二象限，3表示第三象限，4表示第四象限，0位于居中位置
      Comeback: 0,               //返回岸边标志位，1表示返回岸边

      angle: 0, // 旋转角度
      direction: '', // 旋转方向
      innerLeft: this.outerRadius - this.innerRadius, // 操纵杆内圈的原始left值
      innerTop: this.outerRadius - this.innerRadius, // 操纵杆内圈的原始top值
      isMoving: false, // 是否正在移动
      timer: null // 计时器
    }
  },
  mounted: function(){
      this.outerLeft = 200; // 获取操作杆距离页面左边界的距离
      this.outerTop = 200; // 获取操作杆距离页面上边界的距离
      this.centerX = 200; // 中心点的X坐标
      this.centerY = 200; // 中心点的Y左边
  },
  methods:{
    BankReturn: function(event) {     //按键触发事件，将全局变量Comebakc置为1
      var that = this
      global_.Comeback = 1
      console.log('更改数值');
    },
    onBanScroll: function(e) {
      return;
    },
      // 按住摇杆的时候定时触发
      onMoveStart: function(e) {
        let that = this;
        that.timer = setInterval(function() {
          that.$emit("joystickTouchStart", that.direction);
        }, 500);  // 如果时间想要自定义再自己改改好了
      },
      // 摇杆移动事件
      onJoystickMove: function(e) {
        const {
          clientX,
          clientY
        } = e.touches[0];
        let diffX = clientX - this.centerX; // 触碰点到中心的X距离
        let diffY = clientY - this.centerY; // 触碰点到中心的Y距离
        let edge = Math.sqrt(diffX * diffX + diffY * diffY); // 触碰点到中心的距离
        this.isMoving = true;

        // 如果触碰点在范围内
        if (edge <= this.outerRadius) {
          this.innerLeft = Math.round(clientX - this.outerLeft - this.innerRadius);
          this.innerTop = Math.round(clientY - this.outerTop - this.innerRadius);
        } else {
          // 接触点在范围外
          let ratio = this.outerRadius / edge;
          this.innerLeft = Math.round(diffX * ratio + this.innerRadius);
          this.innerTop = Math.round(diffY * ratio + this.innerRadius);
        }
        this.getAngle(diffX, diffY); // 计算角度
        this.$emit("JoystickTouchMove", this.direction);
      },

      // 计算角度
      getAngle: function(diffX, diffY) {
        let edge = Math.sqrt(diffX * diffX + diffY * diffY);
        // console.log(edge);
        if (edge !== 0) {
          let cos = diffX / edge;
          let angle = Math.acos(cos);
          let oldDirection = this.direction; // 获取旧的方向
          this.angle = diffY > 0 ? 360 - angle * 180 / Math.PI : angle * 180 / Math.PI;
          angle = this.angle;
          let newDirection = '';
          if (angle < 22.5 && angle >= 0 || angle < 360 && angle >= 337.5) {
            newDirection = '右';
          } else if (angle < 22.5 * 3 && angle >= 22.5 * 1) {
            newDirection = '右上';
          } else if (angle < 22.5 * 5 && angle >= 22.5 * 3) {
            newDirection = '上';
          } else if (angle < 22.5 * 7 && angle >= 22.5 * 5) {
            newDirection = '左上';
          } else if (angle < 22.5 * 9 && angle >= 22.5 * 7) {
            newDirection = '左';
          } else if (angle < 22.5 * 11 && angle >= 22.5 * 9) {
            newDirection = '左下';
          } else if (angle < 22.5 * 13 && angle >= 22.5 * 11) {
            newDirection = '下';
          } else if (angle < 22.5 * 15 && angle >= 22.5 * 13) {
            newDirection = '右下';
          }
          // 方向改变时才触发
          if(newDirection !== oldDirection) {
            this.direction = newDirection;
            this.$emit("joystickAngleChange", {
              direction: this.direction,
              angle
            });  // 触发外部事件并返回返回方向和角度
            console.log(this.direction)
          }
        }
      },


      // 离开摇杆后摇杆返回中心点
      joystickRestore: function(e) {
        this.isMoving = false;
        this.innerLeft = this.outerRadius - this.innerRadius;
        this.innerTop = this.outerRadius - this.innerRadius;
        this.direction = '';
        this.angle = 0;
        this.$emit("joystickTouchEnd");  // 触发停止事件
        clearInterval(this.timer);
      }
  },

  created () {
    
  },
  onLoad: function(options){    //接收页面传参
    var that = this
    that.storage = global_.storage
    that.voltage = global_.voltage
  },
    
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
  .title-control{
    text-align:center;
    align-items:center;
    justify-content: center;
    padding: 10px;
    margin-top: 20px;
    margin-bottom: 10px;
    font-size: 20px;
    color: #7f7f7f;
  }
  .roter1{
    text-align:center;
    align-items:center;
    justify-content: center;
    padding: 0px;
    margin-left: 80px;
    margin-top: 50px;
    margin-bottom: 50px;
    font-size: 20px;
  }
    .rj-joystick-container {
    display: flex;
    justify-content: center;
    align-items: center;
    position: relative;

    .outer-view {
      background: rgb(227, 232, 233);
      position: relative;
      border-radius: 50%;
      box-sizing: border-box;
      box-shadow: 0 0 5rpx rgb(213, 218, 219) inset;
      border: 1px solid #fff;

      .inner-view {
        background: linear-gradient(to bottom, rgb(255, 255, 255), rgb(234, 244, 254));
        position: absolute;
        border-radius: 50%;
        box-shadow: 0 0 16rpx rgb(213, 218, 219);
        border: 1px solid rgb(217, 221, 228);
        box-sizing: border-box;
      }

      .un-move {
        transition: all .4s;
      }

      .control-direction {
        position: absolute;
        font-size: 60rpx;
        color: #bbb;
        width: 50%;
        height: 70rpx;
        top: calc(50% - 35rpx);
        left: 50%;
        text-align: right;
        line-height: 70rpx;
        padding-right: 30rpx;
        display: block;
        box-sizing: border-box;
        transform-origin: 0 50%;
      }

      text {
        &:nth-child(0) {
          transform: rotate(0deg);
        }

        &:nth-child(1) {
          transform: rotate(90deg);
        }

        &:nth-child(2) {
          transform: rotate(180deg);
        }

        &:nth-child(3) {
          transform: rotate(270deg);
        }
      }
    }
  }
}
</style>
