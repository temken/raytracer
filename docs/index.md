---
layout: page
title: Home
---

# Raytracer Project

Welcome to the development blog for my raytracer project! This is a physically-based rendering engine built in C++ that simulates light transport to eventually create photorealistic images.

## Recent Blog Posts

{% for post in site.posts limit:5 %}
- [{{ post.title }}]({{ post.url | relative_url }}) - {{ post.date | date: "%B %d, %Y" }}
{% endfor %}
