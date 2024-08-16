# توضیحات در مورد پروژه

## فایل `robots.txt`
این فایل نشان می‌دهد که برای خزش یا کرال کردن در این سایت هر یوزر-ایجنت یا هر ماموری می‌تواند از چه مسیرهایی استفاده کند یا از چه مسیر‌هایی استفاده نکند.
لذا با مقایسه این فایل برای سایت‌های `Houzz` و `Johnlewis` و همچنین اینکه دسترسی به عکس‌ها در سایت `JohnLewis` مشکل‌تر است و الگوی خاصی ندارد، بهتر است که از سایت `Houzz` داده‌های خود را استخراج کنیم.

می‌توانید در `crawler.ipynb` مشاهده کنید که یک کلاس `Crawler` تعریف شده است که چهارچوبی کلی برای خزش در سایت‌های مختلف فراهم می‌کند و بخشی از کد که در همه‌ی خزش‌های ما مشترک است در آن قرار دارد ولی تابع‌های `abstract` این کلاس باید برای خزش هر سایت متفاوت پیاده‌سازی متفاوتی داشته باشد.

با دادن مسیر مناسب به خزشگر می‌توانیم خروجی خواسته شده و همچنین مواردی که در هنگام خزش دچار خطا شده‌اند را در دو فایل `data.json` و `errors.json` مشاهده کنیم.
```json
 {
    "title": "Lavish Home Desk With Hairpin Legs",
    "This Product Has Been Described As": [
      "computer",
      "desk",
      "d\u0329cor",
      "end",
      "furniture",
      "hair",
      "Home",
      "industrial",
      "legs",
      "living",
      "mid-century",
      "modern",
      "nightstand",
      "pin",
      "room",
      "small",
      "steel",
      "table",
      "wood"
    ],
    "thumbnails": [
      "https://st.hzcdn.com/fimgs/5761da810cbf3d25_9102-w65-h65-b1-p0--.jpg",
      "https://st.hzcdn.com/fimgs/3aa1e6b80cbf3d28_0426-w65-h65-b0-p0--.jpg",
      "https://st.hzcdn.com/fimgs/6ec1f14d0cbf3d2b_0426-w65-h65-b1-p0--.jpg",
      "https://st.hzcdn.com/fimgs/12d1caf70cbf3d2d_0426-w65-h65-b1-p0--.jpg",
      "https://st.hzcdn.com/fimgs/0271d05a0cbf3d2f_0426-w65-h65-b1-p0--.jpg"
    ],
    "images": [
      "https://st.hzcdn.com/simgs/5761da810cbf3d25_9-9102/home-design.jpg",
      "https://st.hzcdn.com/simgs/3aa1e6b80cbf3d28_9-9102/home-design.jpg",
      "https://st.hzcdn.com/simgs/6ec1f14d0cbf3d2b_9-9102/home-design.jpg",
      "https://st.hzcdn.com/simgs/12d1caf70cbf3d2d_9-9102/home-design.jpg",
      "https://st.hzcdn.com/simgs/0271d05a0cbf3d2f_9-9102/home-design.jpg"
    ]
  }
```
فایل `data.json` آرایه‌ای از اشیائی به شکل بالا می‌باشد که تیتر و تگ‌ها و عکس‌های کوچک و بزرگ محصول را نشان می‌دهد.

با تنظیم کردن آپشن `items_count` در متد `run` می‌توانیم تعداد موارد خزش شده را مشخص کنیم. همچنین می‌توانیم با تنظیم آپشن `saving_interval` در سازنده‌ی کلاس مشخص کنیم که بعد از خزش هر چند داده فایل `data.json` آپدیت شود. همچین در صورت به وجود آمدن خطا مثل قطع شدن اینترنت یا خطاهای مربوط به `status code` بازگردانده شده از طرف سایت. مواردی که به خطا خورده‌اند نگهداری می‌شوند و می‌توانید با صدا کردن `run_failed_attempts` دوباره سعی کنید تا خزش را درباره آنها ادامه دهید.