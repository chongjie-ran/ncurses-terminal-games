# 视频制作技术方案 - 可灵API禁用应对

> 开发者悟通 | 2026-03-30

---

## 一、核心问题分析

### 当前状态
- 可灵API已被禁用，无法调用 `api-beijing.klingai.com`
- 4个视频处于"私密"状态（生成成功但发布失败）
- 现有5个人物形象素材（PNG/JPG格式）
- 已配置的发布环境：Chrome Profile + douyin-uploader.js

### 需求
- 每日3个视频：早7点、午12点、晚18点
- 视频时长：15-30秒
- 需要AI人物形象

---

## 二、可灵API禁用后的替代方案

### 方案A：即梦AI（字节跳动）⭐推荐

**优势**
- 字节跳动出品，质量对标可灵
- 支持API调用（图生视频+配音）
- 国内访问稳定

**API接入方式**
```bash
# 即梦AI API
# 申请地址：https://jimeng.jianying.com/ai-tool
# SDK: pip install jimengai

python3 -c "
from jimeng import JimengAI
ai = JimengAI(api_key='YOUR_KEY')
result = ai.image_to_video(
    image='avatar.png',
    prompt='现代女性在办公室讲解，眼神自然，手势丰富',
    duration=10,
    ratio='9:16'
)
"
```

**价格参考**
- 免费额度：每日50积分
- 付费：约0.1元/秒

---

### 方案B：海螺AI（字节跳动）

**特点**
- 专门做数字人视频
- 形象+声音一致性更好
- API文档：https://hailuoai.video/

---

### 方案C：通义千问（阿里）

**优势**
- 免费额度大
- 阿里云账号直接使用
- 视频生成+配音一体化

**使用方式**
```bash
# 通义千问开放平台
# https://qwen.ai.cloud.alibaba.com/

curl -X POST "https://qwen.ai/api/video/generate" \
  -H "Authorization: Bearer YOUR_TOKEN" \
  -d '{"image_url": "xxx", "prompt": "描述", "duration": 10}'
```

---

### 方案D：腾讯混元

**特点**
- 支持视频生成
- 与微信/企微生态集成好

---

## 三、本地视频制作工具推荐

### 数字人制作

| 工具 | 费用 | 效果 | 本地部署 |
|------|------|------|----------|
| SadTalker | 免费开源 | ⭐⭐⭐ | ✅ 可本地 |
| Roop | 免费开源 | ⭐⭐⭐ | ✅ 可本地 |
| Wav2Lip | 免费开源 | ⭐⭐⭐⭐ | ✅ 可本地 |
| D-ID | 在线服务 | ⭐⭐⭐⭐⭐ | ❌ |

### 本地视频生成栈

```
人物图片 → SadTalker/Roop → 动态视频
              ↓
         FFmpeg合并
              ↓
         配音合成
              ↓
         最终输出
```

**SadTalker安装**
```bash
# 推荐使用整合包
git clone https://github.com/OpenTalker/SadTalker
cd SadTalker
pip install -r requirements.txt

# 下载预训练模型（约300MB）
bash scripts/download_models.sh
```

**Roop安装**
```bash
pip install roop roop-roop
# 下载inswapper模型
```

---

## 四、自动化视频制作流程（推荐）

### 方案1：混合云方案（3天内可落地）⭐

```
┌─────────────────────────────────────────────────────┐
│                   每日自动化流程                      │
├─────────────────────────────────────────────────────┤
│                                                     │
│  7:00/12:00/18:00 → Cron触发                        │
│         ↓                                          │
│  内容生成（AI脚本）                                  │
│         ↓                                          │
│  即梦AI/可灵网页 → 图生视频+配音                     │
│         ↓                                          │
│  FFmpeg后处理 → 添加字幕/转场                        │
│         ↓                                          │
│  抖音API/浏览器自动化 → 发布                         │
│         ↓                                          │
│  记录到memory/douyin-posts.md                       │
│                                                     │
└─────────────────────────────────────────────────────┘
```

**关键技术栈**
- **AI脚本生成**：Claude/MiniMax API
- **视频生成**：即梦AI（网页自动化或API）
- **视频处理**：FFmpeg + Python
- **发布**：已有Chrome Profile + douyin-uploader.js

---

### 方案2：全本地方案（适合隐私/成本敏感）

**优势**
- 完全免费
- 无API依赖
- 可离线运行

**工具链**
```bash
# 1. 图片生成（新形象）
ComfyUI / Stable Diffusion

# 2. 数字人视频
SadTalker / Wav2Lip

# 3. 配音
MiniMax TTS（已有skill）

# 4. 合并
FFmpeg

# 5. 发布
浏览器自动化
```

**本地部署需求**
- GPU: NVIDIA 3060+（推荐）
- 内存: 16GB+
- 硬盘: 50GB+

---

## 五、3天快速落地方案（低成本）

### Day 1：环境搭建
```
1. 注册即梦AI账号，获取API Key
2. 配置环境变量：
   export JIMENG_API_KEY="xxx"
3. 测试API调用
4. 准备FFmpeg后处理脚本
```

**即梦API测试脚本**
```python
#!/usr/bin/env python3
"""即梦AI视频生成测试"""
import os, requests, base64, time

API_KEY = os.environ.get("JIMENG_API_KEY", "")
API_URL = "https://api.jimengai.com/v1/video/image2video"

def generate_video(image_path: str, prompt: str, output: str):
    with open(image_path, 'rb') as f:
        img_b64 = base64.b64encode(f.read()).decode()
    
    headers = {"Authorization": f"Bearer {API_KEY}", "Content-Type": "application/json"}
    data = {
        "model": "kling-v1",
        "image": img_b64,
        "prompt": prompt,
        "duration": 10,
        "aspect_ratio": "9:16",
        "with_audio": True
    }
    
    resp = requests.post(API_URL, json=data, headers=headers, timeout=30)
    result = resp.json()
    
    if "task_id" in result:
        task_id = result["task_id"]
        # 轮询直到完成
        for _ in range(60):
            time.sleep(5)
            status = requests.get(f"{API_URL}/{task_id}", headers=headers).json()
            if status.get("status") == "completed":
                video_url = status["data"]["video_url"]
                # 下载视频
                video_data = requests.get(video_url).content
                with open(output, 'wb') as f:
                    f.write(video_data)
                return True
    return False

if __name__ == "__main__":
    print("即梦AI视频生成测试")
    print("请设置环境变量 JIMENG_API_KEY")
```

### Day 2：自动化脚本开发
```
1. 开发AI脚本生成模块
2. 开发视频调用封装
3. 开发FFmpeg后处理
4. 开发发布自动化
```

**完整自动化脚本结构**
```
douyin-auto-producer/
├── config.yaml          # 配置（时间、账号、素材）
├── scripts/
│   ├── content_generator.py   # AI生成脚本
│   ├── video_generator.py     # 视频生成调用
│   ├── video_processor.py     # FFmpeg后处理
│   └── publisher.py           # 抖音发布
├── main.py              # 主入口
└── requirements.txt
```

### Day 3：Cron定时任务部署
```bash
# crontab -e
# 每日3个视频
0 7,12,18 * * * /path/to/douyin-auto-producer/main.py >> /var/log/douyin.log 2>&1
```

---

## 六、紧急修复：4个私密视频处理

**问题**：视频生成成功但发布失败，变为私密状态

**原因推测**
- 网络问题导致上传中断
- Cookie过期
- 文件格式问题

**解决方案**
```bash
# 1. 确认视频文件是否还存在
ls -la ~/douyin-videos/*.mp4 | grep -E "2026-03-2[89]|2026-03-30"

# 2. 重新上传已生成的视频
# 使用已有的douyin-uploader.js
node ~/.openclaw/workspace/skills/douyin-upload/douyin-uploader.js \
  --video /path/to/video.mp4 \
  --title "标题" \
  --cookie "$(cat ~/.openclaw/workspace/skills/douyin-upload/douyin-cookies.json)"

# 3. 如果视频丢失，重新生成
# 使用新的AI视频服务
```

---

## 七、成本对比

| 方案 | 每日3视频成本 | 启动成本 | 稳定性 |
|------|---------------|----------|--------|
| 可灵API（已禁用） | ~3元/天 | - | - |
| 即梦AI | ~3元/天 | 低 | ⭐⭐⭐⭐ |
| 全本地(SD+SadTalker) | 0 | 高(GPU) | ⭐⭐⭐ |
| 混合方案(即梦+本地) | ~2元/天 | 中 | ⭐⭐⭐⭐ |

---

## 八、推荐执行路径

### 立即行动（今天）
1. **注册即梦AI**，获取API Key
2. **测试视频生成**，验证效果
3. **重新发布4个私密视频**

### 本周内
1. 开发完整自动化脚本
2. 配置Cron定时任务
3. 监控视频质量

### 长期优化
1. 考虑本地部署SadTalker/Roop
2. 储备多个AI视频源（容灾）

---

## 九、关键文件位置

| 用途 | 路径 |
|------|------|
| 人物形象素材 | `~/douyin-videos/characters/` |
| 视频输出目录 | `~/douyin-videos/` |
| 抖音上传脚本 | `~/.openclaw/workspace/skills/douyin-upload/` |
| 发布记录 | `~/.openclaw/workspace/memory/douyin-posts.md` |

---

## 十、建议的下一步行动

1. **授权悟通**：开发即梦AI对接脚本，3小时内完成基础对接
2. **授权悟通**：开发自动化视频制作Pipeline
3. **紧急**：重新上传4个私密视频

---

*如需进一步技术细节或实施帮助，请告知。*
