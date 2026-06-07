param(
    [string]$Path = ".",
    [int]$SpacesPerTab = 4
)

function Convert-FileEncoding {
    param([string]$File)

    Write-Host "Processing $File"

    # 원본 읽기 (줄 단위, 원래 인코딩 무시)
    $rawContent = Get-Content $File -Raw

    # 탭 -> 스페이스 변환
    $space = " " * $SpacesPerTab
    $rawContent = $rawContent -replace "`t", $space

    # 줄 끝 변환 (CRLF → LF)
    $rawContent = $rawContent -replace "`r`n", "`n"
    $rawContent = $rawContent -replace "`r", "`n"

    # 파일 끝에 개행 보장
    if (-not $rawContent.EndsWith("`n")) {
        $rawContent += "`n"
    }

    # UTF-8 with BOM으로 저장
    $utf8Bom = New-Object System.Text.UTF8Encoding $true
    [System.IO.File]::WriteAllText($File, $rawContent, $utf8Bom)
}

# .h / .cpp / .cs / .ini 다 변환
Get-ChildItem -Path $Path -Recurse -Include *.h, *.cpp, *.cs, *.ini | ForEach-Object {
    Convert-FileEncoding $_.FullName
}

Write-Host "Change Commited: $Path"