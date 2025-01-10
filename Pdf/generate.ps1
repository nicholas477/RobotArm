$scriptpath = $MyInvocation.MyCommand.Path
$dir = Split-Path $scriptpath
Set-Location -Path $dir

Write-Output "Hello World"

& pdflatex ./Manual.tex