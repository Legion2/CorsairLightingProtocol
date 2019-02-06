workflow "Issue Checker" {
  resolves = ["Issue Checklist Checker"]
  on = "issues"
}

action "Issue Checklist Checker" {
  uses = "adamzolyak/checklist-checker-action@1.1.1"
  secrets = ["GITHUB_TOKEN"]
}

workflow "Test" {
  on = "push"
  resolves = ["Build examples"]
}

action "Install FastLED" {
  uses = "actions/bin/sh@master"
  args = ["mkdir -p libraries", "cd libraries; wget -O FastLED.zip https://github.com/FastLED/FastLED/archive/3.2.0.zip; unzip FastLED.zip; rm FastLED.zip"]
}

action "Build examples" {
  uses = "Legion2/arduino-builder-action@master"
  needs = ["Install FastLED"]
}
