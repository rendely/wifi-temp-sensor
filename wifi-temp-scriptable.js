// Variables used by Scriptable.
// These must be at the very top of the file. Do not edit.
// icon-color: blue; icon-glyph: magic;

chart_url = 'https://docs.google.com/spreadsheets/d/e/<your chart link>&format=image'
chart = await new Request(chart_url).loadImage()

w = main()

if (config.runsInWidget) {
    Script.setWidget(w)
    Script.complete()
  } else {
    w.presentMedium()
  }

function main(){
  let w = new ListWidget()
  w.backgroundColor = Color.black()
  w.setPadding(10,10,10,10)
  s = w.addStack()
  s.addSpacer()
  s.addImage(chart)
  s.addSpacer()
  return w
}