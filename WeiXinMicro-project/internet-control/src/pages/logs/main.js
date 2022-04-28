import Vue from 'vue'
import App from './index'
import PeachJoystick from '@/components/Joystick'
Vue.use(PeachJoystick);
Vue.component('peach-joystick',PeachJoystick);

const app = new Vue(App)
app.$mount()

