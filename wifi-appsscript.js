function doGet(e) {
  if (!e.parameter.temp) {
    var sheet_name = 'Data'
    var sheet = SpreadsheetApp.getActiveSpreadsheet().getSheetByName(sheet_name);
    var data_range = sheet.getDataRange()
    var data = data_range.getValues()
    var temp = data[data.length-1][1];
    console.log(temp);
    return ContentService.createTextOutput(JSON.stringify({"temp": temp}) ).setMimeType(ContentService.MimeType.JSON);     
  }else{    
    var temp = e.parameter.temp    
    var sheet_name = 'Data'
    var sheet = SpreadsheetApp.getActiveSpreadsheet().getSheetByName(sheet_name);
    var label_formula = "=if(ISBLANK(INDIRECT(\"R[1]C[-1]\",false)),INDIRECT(\"R[0]C[-1]\",false),\"\")"
    sheet.appendRow([new Date(),temp,label_formula])
    return ContentService.createTextOutput('Success')
  } 
}