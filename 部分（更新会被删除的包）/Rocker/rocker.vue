<template>
  <view class="rj-joystick-container" id="rj-joystick" :style="{ width: outerRadius * 2 + 'px', height: outerRadius * 2 + 'px'}">
    <view class="outer-view" :style="{ width: outerRadius * 2 + 'px', height: outerRadius * 2 + 'px'}">
      <text v-for="n in 4" :key="n" class="control-direction cuIcon-right lg text-gray">></text>
      <view class="inner-view" :class="{ 'un-move': !isMoving }" :style="{ width: innerRadius * 2 + 'px', height: innerRadius * 2 + 'px', left: innerLeft + 'px', top: innerTop + 'px'}"
       @touchmove="onJoystickMove" @touchend="joystickRestore" @touchstart="onMoveStart">
      </view>
    </view>
  </view>
</template>

<script>
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

    data() {
      return {
        angle: 0, // 旋转角度
        direction: '', // 旋转方向
        innerLeft: this.outerRadius - this.innerRadius, // 操纵杆内圈的原始left值
        innerTop: this.outerRadius - this.innerRadius, // 操纵杆内圈的原始top值
        isMoving: false, // 是否正在移动
        timer: null // 计时器
      };
    },

    mounted: function() {
      const query = wx.createSelectorQuery().in(this);
      query.select('#rj-joystick').boundingClientRect(data => {
        this.outerLeft = data.left; // 获取操作杆距离页面左边界的距离
        this.outerTop = data.top; // 获取操作杆距离页面上边界的距离
        this.centerX = data.left + this.outerRadius; // 中心点的X坐标
        this.centerY = data.top + this.outerRadius; // 中心点的Y左边
      }).exec();
    },

    methods: {
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
    }
  }
</script>

<style lang="scss" scoped>
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
</style>

