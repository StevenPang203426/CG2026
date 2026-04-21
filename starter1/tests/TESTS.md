# Assignment 1 – Test Cases

每个测试先打开**你自己的实现**，关闭窗口后再打开**参考答案**，便于逐一对比。

---

## 运行方式

```bash
# 在 starter1/ 目录下执行
cd starter1

# 运行全部测试
bash tests/run_tests.sh

# 只运行某一个（名称包含关键字即可）
bash tests/run_tests.sh florus
bash tests/run_tests.sh wineglass
```

---

## 测试列表

### 1. circles
**考察点**：Bezier / B-spline 曲线评估，圆形轮廓的旋转曲面（SurfRev）。

```bash
build/a1 swp/circles.swp && sample_solution/athena/a1 swp/circles.swp
```

---

### 2. core
**考察点**：基础曲线与曲面构造，核心控制点布局。

```bash
build/a1 swp/core.swp && sample_solution/athena/a1 swp/core.swp
```

---

### 3. gentorus
**考察点**：GenCyl（广义圆柱面）——沿封闭扫掠曲线展开剖面曲线，生成圆环体。

```bash
build/a1 swp/gentorus.swp && sample_solution/athena/a1 swp/gentorus.swp
```

---

### 4. norm
**考察点**：曲面法向量方向是否正确（正面朝外），背面剔除是否正常。

```bash
build/a1 swp/norm.swp && sample_solution/athena/a1 swp/norm.swp
```

---

### 5. tor
**考察点**：圆环（Torus）旋转曲面，验证旋转角度与顶点密度。

```bash
build/a1 swp/tor.swp && sample_solution/athena/a1 swp/tor.swp
```

---

### 6. weird
**考察点**：非标准控制点配置，检查实现的鲁棒性。

```bash
build/a1 swp/weird.swp && sample_solution/athena/a1 swp/weird.swp
```

---

### 7. weirder
**考察点**：更复杂的非标准曲面，进一步压测边界条件。

```bash
build/a1 swp/weirder.swp && sample_solution/athena/a1 swp/weirder.swp
```

---

### 8. wineglass
**考察点**：酒杯形状，旋转曲面 + 较多控制点，检查轮廓平滑度。

```bash
build/a1 swp/wineglass.swp && sample_solution/athena/a1 swp/wineglass.swp
```

---

### 9. flircle
**考察点**：GenCyl 沿圆形扫掠曲线，剖面为花形曲线，帧传播稳定性。

```bash
build/a1 swp/flircle.swp && sample_solution/athena/a1 swp/flircle.swp
```

---

### 10. florus
**考察点**：Florus（花环）——GenCyl 最复杂示例，剖面曲线必须在 xy 平面内，
验证 Gram-Schmidt 初始化与封闭曲线帧修正（`fixClosedCurve`）。

```bash
build/a1 swp/florus.swp && sample_solution/athena/a1 swp/florus.swp
```

---

## 检查要点

| 检查项 | 说明 |
|--------|------|
| 曲线形状 | 与参考答案的曲线走向、控制点连线一致 |
| 曲面光滑度 | 无明显褶皱或法向量跳变 |
| 法向量方向 | 按 `S` 键显示法线，应均匀朝外 |
| 封闭曲面接缝 | gentorus / florus 的起止处无裂缝 |
| 光照效果 | 白色漫反射 + Blinn-Phong 高光，暗面有环境光 |
