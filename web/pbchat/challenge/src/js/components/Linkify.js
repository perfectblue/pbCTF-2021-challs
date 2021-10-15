import React from "react";
import Arg from "@vunamhung/arg.js";

const URL_REGEX = /((?:[\w]+):(?:\/\/)?(?:(?:[^/\s]+\.?)+)(?:[^#?\s]+)?(?:\?[^#\s]+)?(?:#[^\s]+)?)/gi;

export default function Linkify({ text }) {
  const addTracking = (url) => {
    const u = new URL(url);
    u.search = Arg.stringify({
      ...Arg.parse(u.search),
      utm_source: "pbchat",
      utm_content: "link",
    });
    return u.toString();
  };

  const parse = (t) => {
    return t.split(URL_REGEX).map((part, i) => {
      if (part.match(URL_REGEX)) {
        const url = addTracking(part);
        let checkurl = new URL(url);

        if (checkurl.protocol === "https:" && checkurl.hostname.endsWith("www.youtube.com")){
            checkurl = "https://www.youtube.com/embed/" + checkurl.searchParams.get("v");
            return (
              <iframe sandbox="allow-scripts allow-same-origin allow-presentation" key={i} src={checkurl} className="preview mt-3">
              </iframe>

            );
        }
        else {
          return (
            <a key={i} href={url}>
              {part}
            </a>
          );
        }
      } else {
        return <span key={i}>{part}</span>;
      }
    });
  };

  return <span>{parse(text || "")}</span>;
}
