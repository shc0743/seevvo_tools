# seevvo_tools
A series of utilities that can be used on seevvo or other devices to improve experiences in school

## NtShutdownSystem
快速关机工具，命令行执行 ` NtShutdownSystem r ` 也可以重启

**注意**: 可以加快启用了还原系统(e.g. 冰点)电脑的关机速度，不建议在正常电脑上使用，否则可能造成数据丢失

## SeewoTimelyFuck
定时干掉希沃管家，以实现在知道自动关机时间的前提下，可以全自动阻止自动关机

**注意**: 如果需要去除干掉希沃的提示，可在程序同级目录下创建 `<程序名>.exe.prevent_ixmessages` 文件（只要文件存在就行）

**提示**: customized-notification-window库位于[MyLearn](https://github.com/shc0743/MyLearn/tree/main)

# License
所有本repo下代码，如无特殊说明，都是[GPL-3.0](./LICENSE)许可



