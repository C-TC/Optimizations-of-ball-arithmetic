# You may need to run it several times and choose the best one

---

# How to run it

~~~bash
bash run.sh
~~~

# Change plot accordingly

---

### open timing\_mul\_vec\_p.py


2.   看一下是否是16x speedup，（`python3 timing_mul_vec_p.py` 会输出两个数字，代表大致的speedup）
3. line 66, 看一下16x speedup的箭头位置是否合适，是否需要调整
2.  line 68, clang 12.0.0 => your compiler version 
3. `python3 timing_mul_vec_p.py` 重新生成图片