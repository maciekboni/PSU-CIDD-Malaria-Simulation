for ($num = 0 ; $num -le 9 ; $num++) { 
  & ".\DxGGenerator.exe" -i "EC50_profile_$num.yml" -t 0 10 > "EfficacyTable_$num.csv"
}