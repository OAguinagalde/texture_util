if (!(Test-Path "./release")) { New-Item -Type Directory "release" }
Remove-Item "./release/main.exe" -ErrorAction SilentlyContinue
clang main.c -o "./release/main.exe" -O2
$git_lastTag = (git describe --tags --abbrev=0)
if ($git_lastTag) {
    $git_lastCommitMessages = (git log --pretty=format:%s $git_lastTag`..HEAD)
}
if ($git_lastCommitMessages) {
    Remove-Item "./release/release.md" -ErrorAction SilentlyContinue
    $release_message = ""
    $release_message += "Changelog:`n"
    Foreach ($commit_message in $git_lastCommitMessages) {
        $release_message += "* $commit_message`n"
    }
    New-Item "./release/release.md" -Value $release_message
    $release_files = @()
    $release_files += Get-Item "./release/release.md"
    $release_files += Get-Item "./release/main.exe"
    Remove-Iteam "\release\Texture_util.zip" -ErrorAction SilentlyContinue
    Compress-Archive $release_files -DestinationPath ".\release\Texture_util.zip"
}
