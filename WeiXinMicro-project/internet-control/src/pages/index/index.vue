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


export default {
  data () {
    return {
    }
  },

  components: {
  },

  methods: {

  },

  created () {
    // let app = getApp()
  }
}
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
