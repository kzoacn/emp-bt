# emp-arith [![Build Status](https://kzoacn.visualstudio.com/emp-arith/_apis/build/status/kzoacn.emp-arith?branchName=master)](https://kzoacn.visualstudio.com/emp-arith/_build/latest?definitionId=1&branchName=master)

<img src="https://raw.githubusercontent.com/emp-toolkit/emp-readme/master/art/logo-full.jpg" width=300px/>

## Installation

   1. Install prerequisites using instructions [here](https://github.com/kzoacn/emp-readme).
   2. git clone https://github.com/emp-toolkit/emp-arith.git
   3. cd emp-arith && cmake . -DBOOST_ROOT=/usr/include/boost && sudo make install

## Test

* ./bin/sw_plain是明文的计算, ./bin/sw是加密的

* 在本地运行两方计算

   `./execute ./bin/sw`
* 若要在两台设备之间计算,修改./config中的配置文件中的ip,填入对方的设备ip

  `./bin/sw alice.json` on one machine and 
  
  `./bin/sw bob.json` on the other.
  
* 输入数据也在配置文件中可以修改
