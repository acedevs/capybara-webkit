#include "Evaluate.h"
#include "WebPage.h"
#include "WebPageManager.h"
#include "JsonSerializer.h"
#include <iostream>

Evaluate::Evaluate(WebPageManager *manager, QStringList &arguments, QObject *parent) : SocketCommand(manager, arguments, parent) {
}

void Evaluate::start() {
  QString script = arguments()[0];
  QString jsonArgs;
  if (arguments().length()>1){
    jsonArgs = arguments()[1];
  } else {
    jsonArgs ="[]";
  }
  QString eval_script = QString("(function(){"
                           "   for(var i=0; i<arguments.length; i++) {"
                           "     arguments[i] = JSON.parse(arguments[i]);"
                           "     if (arguments[i]['ELEMENT']) {"
                           "       arguments[i] = Capybara.getNode(arguments[i]['ELEMENT']);"
                           "     };"
                           "   };"
                           "   return eval(\"%1\");"
                           "   }).apply(null, %2)").arg(script.replace("\"","\\\"").remove("\n"), jsonArgs);
  QObject invocation_stub;
  invocation_stub.setProperty("allowUnattached", false);
  page()->currentFrame()->addToJavaScriptWindowObject("CapybaraInvocation", &invocation_stub);
  QVariant result = page()->currentFrame()->evaluateJavaScript(eval_script);
  JsonSerializer serializer;
  finish(true, serializer.serialize(result));
}
