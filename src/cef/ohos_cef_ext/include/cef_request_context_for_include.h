  ///
  /// Clears the client authentication certificate Cache that were added
  /// as part of handling CefRequestHandler::OnSelectClientCertificate().
  /// If |callback| is non-NULL it will be executed on the UI thread after
  /// completion.
  ///
  /*--cef(optional_param=callback)--*/
#if BUILDFLAG(ARKWEB_CERT_AUTHENTICATION)
  virtual void ClearClientAuthenticationCache(
      CefRefPtr<CefCompletionCallback> callback) {};
#endif  // ARKWEB_CERT_AUTHENTICATION

  virtual CefRefPtr<CefWebStorage> GetWebStorage(
      CefRefPtr<CefCompletionCallback> callback) { return nullptr; };