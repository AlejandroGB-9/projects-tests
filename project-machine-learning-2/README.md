# Grupo9-Practica2
Cosas importantes a saber:
  Target -> Attrition
  Datos -> Desbalanceados
  Imputar datos -> missing values
  Partición estratificada -> StratifiedKFold
  Métricas -> balanced_accuracy y f1 y plot de la matriz de confusión
  Utilizar class_weight = "balanced"
  Los métodos son de clasificación
  modelo:
    LosgisticRegression() -> sin hiper parámetros
    Boosting -> Ajustar hiper parámetros utilizando las métricas anteriores
      -> Usar pipelines, métodos de boosting y librerías externas (+0.35 p) del enunciado (lightgbm - Microsoft, preferible)
    Para el punto 5 se añade lo pedido en el pipeline
    El uso de pipeline es mejor ya que es reitilizable
  
Se entrega:
  Un notebook con explicaciones y los modelos, incluido la parte que hace el archivo .pkl al seleccionar el modelo final
