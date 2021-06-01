# You may need to run it several times and choose the best one

---

# How to run it

~~~bash
bash run.sh
~~~

# Change plot accordingly

---

### open timing\_add\_2.py


2. line 37, clang 12.0.0 => your compiler version 
3. `python3 timing_add_2.py` 重新生成图片，应该有个4x speedup

### open timing\_add\_3.py

1. line33,35,37:确认cache标注的位置是否合适
2. 确认真的有2.7x speedup （`python3 timing_add_3.py`会输出这speedup数字）
3. line 38:确认2.7xspeedup箭头的位置恰当
2. line 41, clang 12.0.0 => your compiler version 
3. `python3 timing_add_3.py` 重新生成图片