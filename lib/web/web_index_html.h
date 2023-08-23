/*
 * @Description: 
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-08-22 13:54:14
 * @LastEditTime: 2023-08-23 10:09:48
 */

/***********************************************************************************************
 * 版权声明：
 * 本源代码的版权归 [saisaiwa] 所有。
 *
 * 未经版权所有者明确授权，不得将本代码的任何部分用于商业用途，包括但不限于出售、出租、许可或发布。
 * 仅限个人学习、研究、非盈利性用途下使用。如果您有其他用途的需求，请联系 [yustart@foxmail.com] 进行授权。
 *
 * 在遵循以下条件的情况下，您可以自由修改、使用和分发本代码：
 * - 您必须保留此版权声明的所有内容。
 * - 您不得用于任何违法、滥用、恶意的活动。
 * - 您的使用不应损害任何个人、组织或作者的声誉。
 *
 * 作者不承担因使用本代码而产生的任何担保责任。作者对因使用本代码所导致的任何直接或间接损失不承担责任。
 * Github: https://github.com/ccy-studio/CCY-VFD-7BT317NK
 ***********************************************************************************************/

/*
 * @Description: 
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2023-08-18 14:52:18
 * @LastEditTime: 2023-08-21 22:58:46
 */
#ifndef __HTML_INDEX_H
#define __HTML_INDEX_H
#include <Arduino.h>
const char HTML_INDEX[] PROGMEM = R"HTML(<!doctype html><html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>VFD后台配置</title><link rel="stylesheet" href="https://unpkg.com/element-ui/lib/theme-chalk/index.css"><style>html{font-family:helvetica neue,Helvetica,pingfang sc,hiragino sans gb,microsoft yahei,微软雅黑,Arial,sans-serif;background-color:#eee}body{margin:0}.header{background-color:#fff;text-align:center;display:flex}.header span{flex:1;align-self:center;font-size:18px;font-weight:700}.bq{font-size:12px;color:#909399}</style></head><body><div id="app"><el-container><el-header class="header"><span>VFD桌面时钟后台配置 By:SAISAIWA</span></el-header>
<el-main><el-alert effect="dark" style="margin-bottom: 20px;" title="修改完记得点击保存按钮" type="success"></el-alert>
<el-card class="box-card"><el-form ref="form" :model="form" label-width="100px" label-position="top"><el-form-item label="G1动画"><el-switch v-model="form.annoOpen"></el-switch></el-form-item>
<el-form-item label="RGB灯效"><el-switch v-model="form.rgbOpen"></el-switch></el-form-item>
<el-form-item label="RGB灯效样式" v-if="form.rgbOpen"><el-radio-group v-model="form.rgbStyle"><el-radio :label="1">渐变呼吸</el-radio>
<el-radio :label="2">彩虹渐变</el-radio>
<el-radio :label="3">恐怖阴森</el-radio></el-radio-group></el-form-item>
<el-form-item label="RGB亮度" v-if="form.rgbOpen"><el-slider :show-tooltip="false" v-model="form.rgbBrightness" :min="15" :max="255"></el-slider>
<el-tag effect="dark" type="primary">亮度百分比:
{{Math.floor(form.rgbBrightness/255*100)}}%</el-tag></el-form-item>
<el-form-item label="滚动文字"><span slot="label">滚动文字<el-tag effect="plain" style="margin-left: 5px;" size="mini" type="info">英文数字,英文下的标点符号。不支持中文</el-tag></span>
<el-input maxlength="20" show-word-limit v-model="form.customLongText" placeholder="英文表单符号数字" autosize clearable></el-input><p style="font-size:12px;color:#f56c6c">请勿在滚动模式下修改滚动文字,否则可能会出现错误!</p></el-form-item><el-form-item label="滚动文字帧率(ms)"><el-input @change="customFrameCheckChange" v-model="form.customLongTextFrame" placeholder="输入帧率50~300" type="number" clearable></el-input></el-form-item>
<el-form-item label="定时开关机"><el-switch v-model="form.autoPower"></el-switch></el-form-item>
<el-form-item label="开机时间设定" v-if="form.autoPower"><el-time-picker value-format="HH:mm:ss" clearable v-model="form.autoPowerOpenTime" placeholder="任意时间点"></el-form-item><el-form-item label="关机时间设定" v-if="form.autoPower"><el-time-picker clearable v-model="form.autoPowerCloseTime" value-format="HH:mm:ss" placeholder="任意时间点"></el-time-picker></el-form-item>
<el-form-item label="开关机生效条件" v-if="form.autoPower"><el-checkbox-group v-model="form.autoPowerEnableDays"><el-checkbox :label="1">周一</el-checkbox>
<el-checkbox :label="2">周二</el-checkbox>
<el-checkbox :label="3">周三</el-checkbox>
<el-checkbox :label="4">周四</el-checkbox>
<el-checkbox :label="5">周五</el-checkbox>
<el-checkbox :label="6">周六</el-checkbox>
<el-checkbox :label="0">周天</el-checkbox></el-checkbox-group></el-form-item>
<el-form-item label="闹钟"><el-switch v-model="form.alarmClock"></el-switch>
<el-time-picker style="margin-left: 10px;" v-if="form.alarmClock" clearable value-format="HH:mm:ss" v-model="form.alarmClockTime" placeholder="任意时间点"></el-form-item><el-form-item label="闹钟生效条件" v-if="form.alarmClock"><el-checkbox-group v-model="form.alarmClockEnableDays"><el-checkbox :label="1">周一</el-checkbox>
<el-checkbox :label="2">周二</el-checkbox>
<el-checkbox :label="3">周三</el-checkbox>
<el-checkbox :label="4">周四</el-checkbox>
<el-checkbox :label="5">周五</el-checkbox>
<el-checkbox :label="6">周六</el-checkbox>
<el-checkbox :label="0">周天</el-checkbox></el-checkbox-group></el-form-item>
<el-form-item label="倒计时"><span slot="label">倒计时<el-tag style="margin-left: 5px;" type="primary">{{form.countdown?"待执行":"未设定"}}</el-tag></span>
<el-switch v-model="form.countdown"></el-switch>
<el-time-picker style="margin-left: 10px;" v-if="form.countdown" value-format="HH:mm:ss" clearable :picker-options="{
                                start: '00:00:01',
                                step: '00:00:01',
                                end: '12:59:59'
                              }" v-model="form.countdownTime" placeholder="时分秒"></el-form-item></el-form><el-button :loading="btnLoading" @click="saveSetting" style="margin-top: 30px;" icon="el-icon-check" type="primary" round>保存修改</el-button></el-card></el-main>
<el-footer><p class="bq">版权所有 © saisaiwa 2023~2025。保留所有权利。<br>除非获得 saisaiwa 的明确许可，否则不得以任何形式复制、分发或修改本网页内容。<br>免责声明：本网页的内容仅供参考和教育目的。作者不承担由于使用本网页内容而导致的任何直接或间接损失或损害。使用者需自行承担风险。<br>所有第三方商标、标识和名称仅用于说明目的，并可能是其各自所有者的财产。<br>如有任何疑问，请联系:yustart@foxmail.com</p><el-link href="http://saisaiwa.com" type="primary" target="_blank">Create By SAISAIWA</el-link></el-footer></el-container></div><script src="https://unpkg.com/vue@2/dist/vue.js"></script>
<script src="https://unpkg.com/axios/dist/axios.min.js"></script>
<script src="https://unpkg.com/element-ui/lib/index.js"></script>
<script>var vm=new Vue({el:"#app",data:function(){return{btnLoading:!1,form:{annoOpen:!1,rgbOpen:!1,rgbStyle:1,rgbBrightness:255,customLongText:"Hello World",customLongTextFrame:200,autoPower:!1,autoPowerOpenTime:null,autoPowerCloseTime:null,autoPowerEnableDays:[],alarmClock:!1,alarmClockTime:null,alarmClockEnableDays:[],countdown:!1,countdownTime:null}}},mounted(){this.getSettingData()},methods:{getSettingData(){axios.get("get-setting").then(function(e){var t=e.data;vm.form=t,vm.form.alarmClockEnableDays=t.alarmClockEnableDays.filter(function(e){return e!==0}),vm.form.autoPowerEnableDays=t.autoPowerEnableDays.filter(function(e){return e!==0}),console.log(vm.form)})},saveSetting(){this.btnLoading=!0,this.form.customLongText=this.form.customLongText.trim(),axios({url:"/save-setting",method:"POST",data:this.form}).then(function(e){console.log(e.data),e.data==="success"?(vm.btnLoading=!1,vm.$notify({title:"成功",message:"保存成功",type:"success"})):vm.$notify({title:"错误",message:"系统异常",type:"error"})})},customFrameCheckChange(e){e<50&&(this.form.customLongTextFrame=50),e>300&&(this.form.customLongTextFrame=300)}}})</script></body></html>)HTML";

#endif