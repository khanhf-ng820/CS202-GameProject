---
name: weekly-report-generator
description: Generate, format, and compile weekly project reports for the CS202 OOP course (Plants vs. Zombies) following course guidelines (Weekly Report.md, AI Usage Guidelines.pdf, Policies.pdf). Use when the user asks to create, update, or compile a weekly report (e.g., GroupXX_YY.md, GroupXX_YY.pdf), extract prompts and tasks from Antigravity conversation logs, analyze git commit progress across project branches, format AI Usage Declarations with UTC+7 timestamps, or compile reports with GitHub Markdown CSS styling.
---

# Weekly Report Generator — CS202 OOP Course

This skill guides the creation, maintenance, and compilation of weekly project progress reports for the Plants vs. Zombies OOP course project according to the instructor's rubric, formatting standards, and AI usage declaration policies.

---

## 📋 Core Project & Course Information

* **Group ID**: `Group 53`
* **Group Name**: `Group 53`
* **Project Name**: `Plants vs. Zombies`
* **Team Members**:
  * `25125086 – Nguyễn Phúc Khánh`
  * `25125005 – Từ Hoàng Anh`
* **Official AI Guidelines Link**: `https://drive.google.com/file/d/1l6bO6fog1eM6K4_10oMshg5GawmEkr_o/view?usp=sharing`
* **Deliverables Path**: `Weekly-Reports/GroupXX_YY/`
  * `GroupXX_YY.md` (Weekly report in Markdown)
  * `GroupXX_YY.pdf` (Compiled PDF in GitHub markdown styling)
  * Submission ZIP package: `GroupXX_YY.zip` (containing the `.md`, `.pdf`, and any supplementary assets).

---

## 🛠️ Step-by-Step Report Generation Workflow

### Step 1: Git Commit History Analysis (All Branches)

Always inspect git commits across **ALL branches** strictly within the target week's date range (Monday to Saturday / Sunday to Saturday):

```bash
git log --all --format="%h - %an <%ae> (%ad) : %s" --date=iso --since="YYYY-MM-DD" --until="YYYY-MM-DD"
```

1. **Categorize by Author**:
   * `Khanh Nguyen` / `ngpkhanh08@gmail.com` -> `25125086 – Nguyễn Phúc Khánh`
   * `hoanganhtu0922` / `hoanganhtu0922@gmail.com` -> `25125005 – Từ Hoàng Anh`
2. **Extract Evidence**:
   * List relevant branch names (`main`, `UI`, `bowling-level`, etc.).
   * List commit hashes and commit messages.
   * List modified or newly created source files (`.cpp`, `.h`, `.sh`, `.json`, etc.).
3. **Draft Accomplishment Bullets**:
   * Write concise, humanized bullet points highlighting concrete features, bug fixes, architecture refactors, or tooling improvements.

---

### Step 2: Antigravity Prompt Extraction & Trajectory Filtering

When extracting AI prompts from conversation transcript logs:

1. **Locate Logs**:
   * Transcript files are stored at:
     `<appDataDir>/brain/<conversation-id>/.system_generated/logs/transcript_full.jsonl`
     (or fallback to `transcript.jsonl`).
2. **Filter Out Deleted/Reverted Prompts (Trajectory Rollback Handling)**:
   * As the user interacts with Antigravity 2.0, rolled-back or deleted prompts appear in the raw JSONL before an earlier `step_index`.
   * **Algorithm**: Track an active step list. When reading a step with `step_index <= previous_step_index`, pop all steps from the active list with `step_index >= current_step_index`.
3. **Filter Out Short / Command-Only Prompts**:
   * Omit brief command-only inputs or trivial continuation approvals (e.g. bare `/learn`, `Continue`, `Proceed`, `Proceed with Option A`, `Proceed with implementation_plan.md`).
   * Only retain substantive prompts that specify tasks, requirements, constraints, bug reports, or architectural designs.
4. **Strict Relative File Paths**:
   * NEVER include absolute filesystem paths (e.g. `/Users/admin/...` or `file:///Users/admin/...`) in prompt logs or report text.
   * Strip user home and workspace prefixes to format all paths relative to the project workspace root (e.g., `assets/...`, `include/...`, `.agents/rules/AGENTS.md`, `Weekly Report.md`).
5. **Timezone Conversion to UTC+7**:
   * Timestamps in transcript logs are recorded in UTC+0 (`YYYY-MM-DDTHH:MM:SSZ`).
   * **Convert all prompt timestamps to UTC+7 (+07:00)** by adding 7 hours:
     `2026-07-27T03:35:47Z` -> `2026-07-27T10:35:47+07:00`
6. **Sequential Numbering & Header Synchronization**:
   * Label each prompt explicitly with the authoring member:
     * `- **Member**: Nguyễn Phúc Khánh`
     * `- **Member**: Từ Hoàng Anh`
   * Remove conversation grouping headers and number all prompts sequentially (`#### Prompt 1` through `#### Prompt N`, or `#### Prompt 3.X.1` through `#### Prompt 3.X.N`).
   * Ensure any declared prompt count in section headers (`Total Active Prompts: N`) exactly matches the number of prompt entries.
7. **Top N Selection (If Requested)**:
   * When asked for the top N most important prompts (e.g. top 10), prioritize major feature additions, complex physics/gameplay logic, architectural refactoring, and critical bug fixes over minor formatting tweaks.

---

### Step 3: Weekly Report Document Structure (`GroupXX_YY.md`)

The markdown report must adhere strictly to the following structure and tone:

```markdown
# Weekly Report

# General Information
- Group ID: Group 53
- Group Name: Group 53
- Project Name: Plants vs. Zombies
- Date range: YYYY-MM-DD – YYYY-MM-DD

# Tasks Completed This Week

## 25125086 – Nguyễn Phúc Khánh
- [Task 1 description in humanized tone]
- [Task 2 description in humanized tone]
- Evidence:
  - Git Branches: main, UI, ...
  - Commits:
    - [hash] ([commit message])
  - Files modified/created: [file list]

## 25125005 – Từ Hoàng Anh
- [Task 1 description in humanized tone]
- [Task 2 description in humanized tone]
- Evidence:
  - Git Branches: main, ...
  - Commits:
    - [hash] ([commit message])
  - Files modified/created: [file list]

# AI Usage Declaration
During Week XX, our team used the Google Antigravity platform (with Gemini 3.7 Flash and Claude Opus 4.6 models) to help implement [features, refactoring, bug fixes].
All code and plans generated with AI assistance were carefully reviewed, tested, and integrated by our team members.

Guidelines Link: https://drive.google.com/file/d/1l6bO6fog1eM6K4_10oMshg5GawmEkr_o/view?usp=sharing

For full prompt logs, access timestamps, and validation steps, please see the [Appendix: AI Usage Notes](#appendix-ai-usage-notes).

# Tasks Planned for Next Week

## Nguyễn Phúc Khánh
- [Planned task 1]
- [Planned task 2]

## Từ Hoàng Anh
- [Planned task 1]
- [Planned task 2]

# Issues
1. **[Issue 1 Title]**
   - **Problem**: [Description of problem encountered]
   - **Action**: [Resolution or mitigation taken]
2. **[Issue 2 Title]**
   - **Problem**: [Description of problem encountered]
   - **Action**: [Resolution or mitigation taken]

---

# Appendix: AI Usage Notes

### A. General Metadata
- **AI Tools**: Gemini 3.7 Flash and Claude Opus 4.6
- **Platform**: Google Antigravity platform
- **Access Times (UTC+7)**:
  - YYYY-MM-DD: HH:MM, HH:MM, ...
- **Used by**: Nguyễn Phúc Khánh and Từ Hoàng Anh (collaborative development)
- **Purpose**: Assist in developing [core features, UI, mechanics, debugging].

### B. Prompts and Details of Usage

#### Prompt 1
- **Member**: [Nguyễn Phúc Khánh / Từ Hoàng Anh]
- **Sent At**: YYYY-MM-DDTHH:MM:SS+07:00
- **Prompt Content**:
```text
[Exact prompt text]
```
- **Task Accomplished**: [Humanized description of what was designed, implemented, or resolved]

---

### C. Validation and Integrity
All code updates were reviewed, compiled, and tested locally before committing to git. The multi-platform CI/CD pipeline was run to confirm clean builds across Linux, macOS, and Windows.
```

---

### Step 4: Master AI Usage Declaration (`AI_Usage_Declaration.md`)

When creating or maintaining a master repository-wide `AI_Usage_Declaration.md`:

1. **Modular Section Structure**:
   - Organize prompts by development conversation stream / subsystem (`### 3.1 CMake Automatic Asset Download`, `### 3.2 Plant Entity Implementation`, `### 3.3 Bowling Level Implementation`, etc.).
   - Header metadata per section must specify: `Conversation ID`, `Category / Domain`, `Primary Contributor`, `Date Range (UTC+7)`, and `Total Active Prompts`.
2. **Prompt Record Schema**:
   ```markdown
   #### Prompt 3.X.Y
   - **Member**: [Nguyễn Phúc Khánh / Từ Hoàng Anh]
   - **Sent At**: YYYY-MM-DDTHH:MM:SS+07:00
   - **Prompt Content**:
   ```text
   [Exact prompt text with relative paths]
   ```
   - **Task Accomplished**: [Humanized engineering summary]
   ```
3. **Synthesis & Academic Integrity**:
   - Conclude with an explicit Student Authorship & Architectural Ownership statement and signed declarations.

---

## 🎨 PDF Compilation with Official GitHub Markdown Styling

To compile `GroupXX_YY.md` into `GroupXX_YY.pdf`:

### Method: Standalone Pandoc + GitHub CSS + Headless Chrome

1. **Generate HTML Body**:
   ```bash
   pandoc "Weekly-Reports/GroupXX_YY/GroupXX_YY.md" -o "Weekly-Reports/GroupXX_YY/body.html"
   ```

2. **Assemble Full HTML with GitHub Markdown CSS**:
   Combine the downloaded `github-markdown-light.min.css` (or CDN stylesheet) with `<body class="markdown-body">` and custom print formatting:
   ```html
   <!DOCTYPE html>
   <html>
   <head>
   <meta charset="utf-8">
   <title>GroupXX_YY Weekly Report</title>
   <style>
   /* Include github-markdown-css here */
   @page {
       size: A4;
       margin: 20mm 15mm 20mm 15mm;
   }
   .markdown-body {
       box-sizing: border-box;
       min-width: 200px;
       max-width: 980px;
       margin: 0 auto;
       padding: 10px;
       font-size: 13px;
       line-height: 1.6;
   }
   .markdown-body pre {
       page-break-inside: avoid;
       background-color: #f6f8fa;
       border-radius: 6px;
       padding: 16px;
       overflow: auto;
       font-size: 85%;
       line-height: 1.45;
   }
   .markdown-body code {
       background-color: rgba(175,184,193,0.2);
       padding: 0.2em 0.4em;
       border-radius: 6px;
       font-size: 85%;
   }
   .markdown-body pre code {
       background-color: transparent;
       padding: 0;
   }
   .markdown-body h1, .markdown-body h2, .markdown-body h3 {
       page-break-after: avoid;
   }
   </style>
   </head>
   <body class="markdown-body">
   <!-- body.html contents -->
   </body>
   </html>
   ```

3. **Render PDF via Headless Google Chrome**:
   ```bash
   "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome" \
     --headless --disable-gpu --no-pdf-header-footer \
     --print-to-pdf="Weekly-Reports/GroupXX_YY/GroupXX_YY.pdf" \
     "file:///Users/admin/Desktop/HK3 25-26/CS202/Weekly-Reports/GroupXX_YY/GroupXX_YY.html"
   ```

4. **Clean Up Temporary Artifacts**:
   Delete intermediate `body.html` and temporary helper files after PDF generation.

---

## ✍️ Humanized Tone Guidelines

* **Avoid Robotic Repetition**: Do not use generic boilerplate phrases (e.g., "The model assisted the user in generating...") repeatedly across every prompt.
* **Focus on Engineering Context**: Frame accomplishments in terms of real game development challenges — such as coordinate synchronization, asset clipping, frame rate pacing, state machine transitions, and memory allocation safety.
* **Clear Team Member Distinction**: Ensure individual contributions reflect each member's actual domain of work (e.g., UI layout, audio, gameplay levels, build scripts, reanimation interpolation).
