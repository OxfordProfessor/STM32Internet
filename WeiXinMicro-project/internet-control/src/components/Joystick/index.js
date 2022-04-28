import Joystick from './src';

/* istanbul ignore next */
Joystick.install = function(Vue) {
  Vue.component(Joystick.name, Joystick);
};

export default Joystick;