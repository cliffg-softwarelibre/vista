vista_add_doc(vista-doc-circular-span circular/span.adoc)
vista_add_doc(vista-doc-circular-array circular/array.adoc)

add_custom_target(vista-doc-circular
  DEPENDS vista-doc-circular-span
  DEPENDS vista-doc-circular-array
  )
