# Project 2 执行计划：光照模型与光线追踪

## 项目总览

本项目基于 `starter2` 代码框架，实现光线投射（Ray Casting）与递归光线追踪（Ray Tracing）。最终程序能渲染球体、平面、三角形及网格模型，支持 Phong 着色、阴影、反射和抗锯齿。

**需要修改的文件共 4 个：**

| 文件 | 需实现的内容 |
|------|-------------|
| `src/Light.cpp` | `PointLight::getIllumination()` |
| `src/Material.cpp` | `Material::shade()` |
| `src/Object3D.cpp` | `Plane::intersect()`、`Triangle::intersect()`、`Transform` 构造函数及 `intersect()` |
| `src/Renderer.cpp` | `Renderer::traceRay()`（Phong 光照 + 递归光追 + 阴影） |

此外 `src/Object3D.h` 中 `Plane` 和 `Transform` 类需要补充私有成员变量。

---

## 阶段一：Phong 光照模型（任务 2.1）

完成后可渲染 **Scene 1** 中带光照的球体。

### 步骤 1.1 — PointLight::getIllumination()

**文件：** `src/Light.cpp` 第 16-23 行

**当前状态：** 函数体为空，仅有 TODO 注释。`DirectionalLight` 的同名方法已实现可参考。

**实现逻辑：**

```
tolight = (_position - p).normalized()
distToLight = (_position - p).length()
intensity = _color / (1 + _falloff * distToLight^2)
```

- `_position`、`_color`、`_falloff` 均为 `PointLight` 的私有成员，已在 `Light.h` 中定义。
- 光强按距离平方衰减，衰减因子为 `_falloff`。

**验证方式：** 暂时无法单独验证，需配合步骤 1.2、1.3 完成后一起测试。

---

### 步骤 1.2 — Material::shade()

**文件：** `src/Material.cpp` 第 2-9 行

**当前状态：** 直接返回 `Vector3f(0,0,0)`。

**实现逻辑（Phong 着色 = 漫反射 + 镜面反射，不含环境光）：**

1. **漫反射项（Diffuse）：**
   - 计算 `N·L`（法线与光源方向的点积），若 < 0 则漫反射贡献为 0
   - `I_diffuse = _diffuseColor * lightIntensity * max(0, N·L)`
   - 法线 N 从 `hit.getNormal()` 获取，光源方向 L 为参数 `dirToLight`

2. **镜面反射项（Specular）：**
   - 计算反射向量 `R = 2(N·L)N - L`
   - 视线方向 `E = -ray.getDirection().normalized()`
   - `I_specular = _specularColor * lightIntensity * max(0, R·E)^_shininess`
   - 同样仅在 `N·L > 0` 时计算

3. **返回值：** `I_diffuse + I_specular`

**关键变量来源：**
- `_diffuseColor`、`_specularColor`、`_shininess` 来自 `Material.h` 中的成员变量
- 环境光不在此函数中处理，将在 `traceRay()` 中累加

---

### 步骤 1.3 — traceRay() 第一版（Phong 光照）

**文件：** `src/Renderer.cpp` 第 70-85 行

**当前状态：** 相交后直接返回漫反射颜色，无光照计算。

**实现逻辑：**

```
if (intersect) {
    color = ambient_light * material.diffuseColor   // 环境光
    for each light in scene.lights:
        light.getIllumination(hitPoint, toLight, intensity, dist)
        color += material.shade(ray, hit, toLight, intensity)
    return color
} else {
    return scene.getBackgroundColor(r.getDirection())
}
```

**关键 API：**
- `_scene.getAmbientLight()` — 获取环境光
- `_scene.lights` — 光源数组（`std::vector<Light*>`，在 `SceneParser.h` 第 62 行公开）
- `h.getMaterial()` — 获取交点材质
- `r.pointAtParameter(h.getT())` — 获取交点坐标

**验证：** 编译运行 `scene01_plane.txt`，对比 `sample_out/a01.png`。球体应有明显的光照明暗变化。

---

## 阶段二：光线投射 — 几何体相交（任务 2.2）

### 步骤 2.1 — Plane::intersect()

**文件：** `src/Object3D.cpp` 第 81-88 行 + `src/Object3D.h` 第 86-89 行

**需先补充 `Object3D.h` 中 Plane 的私有成员：**

```cpp
Vector3f _normal;
float _d;
```

**构造函数（第 81-83 行）：** 保存 `normal`（归一化）和 `d`。

**intersect 逻辑：**

平面方程 `P·n = d`，光线 `P = o + t*dir`，代入得：

```
t = (d - n·o) / (n·dir)
```

- 若 `n·dir == 0`，光线平行于平面，无交点
- 若 `t > tmin` 且 `t < h.getT()`，更新 Hit：`h.set(t, material, _normal)`

**验证：** 渲染 Scene 1，平面应正确显示。

---

### 步骤 2.2 — Triangle::intersect()

**文件：** `src/Object3D.cpp` 第 89-93 行

**实现方式：Moller-Trumbore 算法**

核心思路：将交点表示为重心坐标 `P = (1-β-γ)A + βB + γC`，同时 `P = o + t*dir`，构建 3x3 线性方程组：

```
[-dir, B-A, C-A] * [t, β, γ]^T = o - A
```

即 `Ax = b` 的形式，用 `Matrix3f::inverse()` 求解。

**判定条件：**
- `β >= 0`，`γ >= 0`，`β + γ <= 1`（点在三角形内）
- `t > tmin` 且 `t < h.getT()`

**法线插值：** 使用重心坐标对三个顶点法线插值：

```
normal = (1-β-γ)*na + β*nb + γ*nc
normal.normalize()
```

**验证：** 渲染 Scene 2（cube），对比 `sample_out/a02.png`。

---

### 步骤 2.3 — Transform 构造函数及 intersect()

**文件：** `src/Object3D.cpp` 第 96-103 行 + `src/Object3D.h` 中 Transform 类

**需在 `Object3D.h` 的 Transform 类中补充私有成员：**

```cpp
Matrix4f _m;      // 世界坐标 <- 局部坐标 变换矩阵
Matrix4f _m_inv;  // 逆矩阵
```

**构造函数：** 保存变换矩阵 `_m` 及其逆 `_m_inv = m.inverse()`。

**intersect 逻辑：**

1. **将光线从世界坐标变换到局部坐标：**
   - `o_local = _m_inv * o_world`（齐次坐标 w=1）
   - `dir_local = _m_inv * dir_world`（齐次坐标 w=0，注意不归一化以保持 t 值语义）

2. **在局部坐标中与子对象求交：**
   - `_object->intersect(localRay, tmin, h)`

3. **将法线变换回世界坐标：**
   - `n_world = (_m_inv.transposed() * n_local).normalized()`
   - 仅取 3x3 部分（前三行三列）进行变换
   - 更新 Hit 中的法线

**验证：** 渲染 Scene 3-5，对比 sample_out。Scene 5（bunny_200）验证网格模型的变换。

---

## 阶段三：递归光线追踪与阴影（任务 2.3）

### 步骤 3.1 — 阴影光线（Shadow Ray）

**文件：** `src/Renderer.cpp`，修改 `traceRay()` 中的光照循环

**实现逻辑：** 在遍历每个光源时，发射阴影光线判断是否被遮挡：

```
for each light:
    light.getIllumination(...)
    if (shadows enabled):
        shadowRay = Ray(hitPoint + epsilon * toLight, toLight)
        Hit shadowHit
        if (scene.intersect(shadowRay, epsilon, shadowHit)):
            if (shadowHit.t < distToLight):
                continue  // 被遮挡，跳过该光源
    color += material.shade(...)
```

**关键细节：**
- `epsilon` 取极小值（如 `1e-4`）防止自相交
- 必须检查 `shadowHit.t < distToLight`，确保遮挡物在光源之前
- 通过 `_args.shadows` 判断是否启用阴影

---

### 步骤 3.2 — 递归反射（Recursive Ray Tracing）

**文件：** `src/Renderer.cpp`，在 `traceRay()` 中添加递归逻辑

**实现逻辑：**

```
if (bounces > 0 && specularColor != (0,0,0)):
    R = dir - 2 * (dir·N) * N          // 反射方向
    reflectRay = Ray(hitPoint + epsilon * R, R)
    Hit reflectHit
    reflectColor = traceRay(reflectRay, epsilon, bounces - 1, reflectHit)
    color += specularColor * reflectColor
```

**关键细节：**
- 反射方向公式：`R = d - 2(d·n)n`，其中 `d` 是入射光线方向（已归一化）
- 递归深度由 `_args.bounces` 控制，每次递归减 1
- `bounces == 0` 时仅做光线投射，不递归
- 反射颜色乘以镜面反射率 `_specularColor`

**验证：**
- Scene 6（bunny_1k）：`-bounces 4`，对比 `sample_out/a06.png`
- Scene 7（arch）：`-shadows -bounces 4`，对比 `sample_out/a07.png`

---

## 阶段四：抗锯齿（选做，1 分）

### 步骤 4.1 — 抖动采样（Jitter Sampling）

**文件：** `src/Renderer.cpp`，修改 `Render()` 函数中的主循环

**实现逻辑：**

当 `-jitter` 参数启用时，对每个像素进行 16 次采样，每次在 ndcx、ndcy 上加随机偏移：

```
for each pixel (x, y):
    color = (0,0,0)
    for i = 0 to 15:
        jitter_x = ndcx + random(-1,1) * pixel_width
        jitter_y = ndcy + random(-1,1) * pixel_height
        ray = cam->generateRay(jitter_x, jitter_y)
        color += traceRay(ray, ...)
    color /= 16
```

其中 `pixel_width = 2.0 / (w - 1)`，`pixel_height = 2.0 / (h - 1)`。

---

### 步骤 4.2 — 高斯滤波（Gaussian Filter）

**文件：** `src/Renderer.cpp`，修改 `Render()` 函数

**实现逻辑：**

当 `-filter` 参数启用时：

1. **上采样：** 以 3 倍分辨率渲染（`w*3 x h*3`）
2. **高斯下采样：** 对每个 3x3 像素块，使用 sigma=1 的高斯核加权求和得到 1 个像素

高斯核（3x3，sigma=1，归一化）：

```
1/16 * | 1  2  1 |
       | 2  4  2 |
       | 1  2  1 |
```

注意边界像素需裁剪滤波核，避免越界访问。

---

## 执行顺序与验证清单

| 序号 | 步骤 | 验证场景 | 预期分值 |
|------|------|---------|---------|
| 1 | PointLight + shade() + traceRay Phong | Scene 1 | 0.6 |
| 2 | Plane::intersect() | Scene 1（完整） | 已含上行 |
| 3 | Triangle::intersect() | Scene 2 | 0.6 |
| 4 | Transform::intersect() | Scene 3, 4, 5 | 1.8 |
| 5 | Shadow Ray | Scene 7 | 0.5 |
| 6 | Recursive Reflection | Scene 6 | 0.5 |
| 7 | Jitter + Filter（选做） | 任意场景 | 1.0 |

**总分：** 必做 4 分 + 选做 1 分 = 5 分（完成度），另有报告 5 分。

## 建议调试策略

1. **先用小分辨率调试（200x200）** ，确认正确后再用高分辨率（800x800）生成报告截图。
2. **每完成一个步骤立即编译测试** ，对比 `sample_out/` 中的参考图像。
3. **善用 `-normals` 和 `-depth` 参数** 生成法线图和深度图辅助排查几何体相交问题。
4. **注意浮点精度** ：阴影光线和反射光线的原点需加 epsilon 偏移（建议 `1e-4`），否则会出现自相交导致的噪点。

## 编译方式

```bash
cd starter2
mkdir build && cd build
cmake ..
make
./a2 -input ../data/scene01_plane.txt -output 01.png -size 200 200
```
