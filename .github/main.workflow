workflow "Issue Checker" {
  resolves = ["Issue Checklist Checker"]
  on = "issues"
}

action "Issue Checklist Checker" {
  uses = "adamzolyak/checklist-checker-action@1.1.1"
  secrets = ["GITHUB_TOKEN"]
}
