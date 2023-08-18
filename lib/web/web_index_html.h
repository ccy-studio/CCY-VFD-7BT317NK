/*
 * @Description: 
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-08-18 14:52:18
 * @LastEditTime: 2023-08-18 17:47:24
 */
#ifndef __HTML_INDEX_H
#define __HTML_INDEX_H
#include <Arduino.h>
const char HTML_INDEX[] PROGMEM = R"HTML(
    <!doctype html><html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>VFD后台配置</title><link rel="stylesheet" href="https://unpkg.com/element-ui/lib/theme-chalk/index.css"><style>html{font-family:helvetica neue,Helvetica,pingfang sc,hiragino sans gb,microsoft yahei,微软雅黑,Arial,sans-serif;background-color:#eee}body{margin:0}.header{background-color:#fff;text-align:center;display:flex}.header span{flex:1;align-self:center;font-size:18px;font-weight:700}.bq{font-size:12px;color:#909399}</style></head><body><div id="app"><el-container><el-header class="header"><span>VFD后台配置-SAISAIWA</span></el-header>
<el-main><el-alert style="margin-bottom: 20px;" title="修改完记得点击保存按钮" type="success"></el-alert>
<el-card class="box-card"><el-form ref="form" :model="form" label-width="100px" label-position="top"><el-form-item label="G1动画"><el-switch v-model="form.annoOpen"></el-switch></el-form-item>
<el-form-item label="RGB灯效"><el-switch v-model="form.rgbOpen"></el-switch></el-form-item>
<el-form-item label="RGB灯效样式" v-if="form.rgbOpen"><el-radio-group v-model="form.rgbStyle"><el-radio label="线性循环" label="1"></el-radio>
<el-radio label="彩虹渐变" label="2"></el-radio></el-radio-group></el-form-item>
<el-form-item label="RGB亮度" v-if="form.rgbOpen"><el-slider :show-tooltip="false" v-model="form.rgbBrightness" :min="5" :max="255"></el-slider>
<el-tag effect="dark" type="success">亮度百分比:
{{Math.floor(form.rgbBrightness/255*100)}}%</el-tag></el-form-item>
<el-form-item label="滚动文字"><el-input v-model="form.customLongText" placeholder="请输入文字" autosize clearable></el-input></el-form-item>
<el-form-item label="滚动文字帧率(ms)"><el-input v-model="form.customLongTextFrame" placeholder="请输入文字" type="number" clearable></el-input></el-form-item>
<el-form-item label="定时开关机"><el-switch v-model="form.autoPower"></el-switch></el-form-item>
<el-form-item label="开机时间设定" v-if="form.autoPower"><el-time-picker clearable v-model="form.autoPowerOpenTime" placeholder="任意时间点"></el-form-item><el-form-item label="关机时间设定" v-if="form.autoPower"><el-time-picker clearable v-model="form.autoPowerCloseTime" placeholder="任意时间点"></el-time-picker></el-form-item>
<el-form-item label="开关机生效条件" v-if="form.autoPower"><el-checkbox-group v-model="form.autoPowerEnableDays"><el-checkbox label="1">周一</el-checkbox>
<el-checkbox label="2">周二</el-checkbox>
<el-checkbox label="3">周三</el-checkbox>
<el-checkbox label="4">周四</el-checkbox>
<el-checkbox label="5">周五</el-checkbox>
<el-checkbox label="6">周六</el-checkbox>
<el-checkbox label="7">周天</el-checkbox></el-checkbox-group></el-form-item>
<el-form-item label="闹钟"><el-switch v-model="form.alarmClock"></el-switch>
<el-time-picker style="margin-left: 10px;" v-if="form.alarmClock" clearable v-model="form.alarmClockTime" placeholder="任意时间点"></el-form-item><el-form-item label="闹钟生效条件" v-if="form.alarmClock"><el-checkbox-group v-model="form.alarmClockEnableDays"><el-checkbox label="1">周一</el-checkbox>
<el-checkbox label="2">周二</el-checkbox>
<el-checkbox label="3">周三</el-checkbox>
<el-checkbox label="4">周四</el-checkbox>
<el-checkbox label="5">周五</el-checkbox>
<el-checkbox label="6">周六</el-checkbox>
<el-checkbox label="7">周天</el-checkbox></el-checkbox-group></el-form-item>
<el-form-item label="倒计时"><span slot="label">倒计时<el-tag style="margin-left: 5px;" type="primary">{{form.countdown?"待执行":"未设定"}}</el-tag></span>
<el-switch v-model="form.countdown"></el-switch>
<el-time-picker style="margin-left: 10px;" v-if="form.countdown" clearable :picker-options="{
                                start: '00:00:01',
                                step: '00:00:01',
                                end: '12:59:59'
                              }" v-model="form.countdownTime" placeholder="时分秒"></el-form-item></el-form><el-button :loading="btnLoading" @click="saveSetting" style="margin-top: 30px;" icon="el-icon-check" type="primary" round>保存修改</el-button></el-card></el-main>
<el-footer><p class="bq">版权所有 © saisaiwa 2023~2025。保留所有权利。<br>除非获得 saisaiwa 的明确许可，否则不得以任何形式复制、分发或修改本网页内容。<br>免责声明：本网页的内容仅供参考和教育目的。作者不承担由于使用本网页内容而导致的任何直接或间接损失或损害。使用者需自行承担风险。<br>所有第三方商标、标识和名称仅用于说明目的，并可能是其各自所有者的财产。<br>如有任何疑问，请联系:yustart@foxmail.com</p><a href="http://saisaiwa.com">Create By SAISAIWA</a></el-footer></el-container></div><script src="https://unpkg.com/vue@2/dist/vue.js"></script>
<script src="https://unpkg.com/axios/dist/axios.min.js"></script>
<script src="https://unpkg.com/element-ui/lib/index.js"></script>
<script>var vm=new Vue({el:"#app",data:function(){return{btnLoading:!1,form:{annoOpen:!1,rgbOpen:!1,rgbStyle:1,rgbBrightness:255,customLongText:"Hello World",customLongTextFrame:200,autoPower:!1,autoPowerOpenTime:null,autoPowerCloseTime:null,autoPowerEnableDays:[],alarmClock:!1,alarmClockTime:null,alarmClockEnableDays:[],countdown:!1,countdownTime:null}}},mounted(){this.getSettingData()},methods:{getSettingData(){axios.get("/get-setting").then(function(e){var t=e.data;console.log(t),this.form=t})},saveSetting(){this.btnLoading=!0,axios({url:"/save-setting",method:"POST",data:this.form}).then(function(e){this.btnLoading=!1,console.log(e.data),this.$notify({title:"成功",message:"保存成功",type:"success"})})}}})</script></body></html>
)HTML";

#endif