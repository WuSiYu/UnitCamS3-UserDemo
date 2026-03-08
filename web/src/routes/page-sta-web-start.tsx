// "use client";

import TitleBar from "../components/title-bar";
import CardStaWebStart from "../components/poster/card-sta-web-start";

export default function PageStaWebStart() {
  return (
    <div className="flex flex-col">
      <TitleBar></TitleBar>

      <div className="flex flex-col mt-24 mb-8 px-2 gap-8 sm:w-3/4 sm:mx-auto">
        <CardStaWebStart></CardStaWebStart>
      </div>
    </div>
  );
}
