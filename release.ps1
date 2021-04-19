if (!(Get-Command "Clang.exe" -ErrorAction SilentlyContinue)) { Write-Error "Clang is not in path."; break; }
if (!(Get-Command "Git.exe" -ErrorAction SilentlyContinue)) { Write-Error "Git is not in path."; return; }
if (!(Test-Path "./release")) { (New-Item -Type Directory "release") > $null }
Remove-Item "./release/main.exe" -ErrorAction SilentlyContinue
clang main.c -o "./release/main.exe" -O2
$git_lastTag = (git describe --tags --abbrev=0)
if ($git_lastTag) {
    $git_lastCommitMessages = (git log --pretty=format:%s $git_lastTag`..HEAD)
    $git_lastCommitMessages_reverse = @()
    for($i = 0; $i -lt $git_lastCommitMessages.Count; $i++) {
        $git_lastCommitMessages_reverse += $git_lastCommitMessages[($git_lastCommitMessages.Count - 1) - $i]
    }
}
if ($git_lastCommitMessages_reverse) {
    Remove-Item "./release/release.md" -ErrorAction SilentlyContinue
    $release_message = ""
    $release_message += "Changelog:`n"

    Foreach ($commit_message in $git_lastCommitMessages_reverse) {
        $release_message += "* $commit_message`n"
    }
    (New-Item "./release/release.md" -Value $release_message) > $null

    if (Get-Command "Glow.exe" -ErrorAction SilentlyContinue) { Glow.exe "./release/release.md" }

    $release_files = @()
    $release_files += Get-Item "./release/release.md"
    $release_files += Get-Item "./release/main.exe"
    Remove-Item ".\release\Texture_util.zip" -ErrorAction SilentlyContinue
    Compress-Archive $release_files -DestinationPath ".\release\Texture_util.zip"
}
