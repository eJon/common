<?php if (!defined('THINK_PATH')) exit();?><!DOCTYPE html>

<html lang="zh-cn">
 <head>
  <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" /> 
  <!-- Meta, title, CSS, favicons, etc. --> 
  <meta charset="utf-8" /> 
  <meta http-equiv="X-UA-Compatible" content="IE=edge" /> 
  <meta name="viewport" content="width=device-width, initial-scale=1" /> 
  
  <!-- Bootstrap core CSS --> 
  <link href="/__debug/Public/css/app.min.css" rel="stylesheet" /> 
  <link href="/__debug/Public/css/font-awesome.min.css" rel="stylesheet" /> 
  <link href="/__debug/Public/css/bootstrap.min.css" rel="stylesheet" /> 
  <script src="/__debug/Public/js/jquery.min.js"></script> 
  <script src="/__debug/Public/js/bootstrap.min.js"></script> 
  <!-- HTML5 shim and Respond.js IE8 support of HTML5 elements and media queries --> 
  <!--[if lt IE 9]>
        <script src="http://cdn.bootcss.com/html5shiv/3.7.0/html5shiv.js"></script>
        <script src="http://cdn.bootcss.com/respond.js/1.4.2/respond.min.js"></script>
      <![endif]--> 
  <!-- Favicons --> 
  <link rel="apple-touch-icon-precomposed" href="./img/apple-touch-icon-precomposed.png" /> 
  <link rel="icon" href="/__debug/Public/images/favion.ico" /> 
  <script>
        var _hmt = _hmt || [];
      </script> 
  <script type="text/javascript">
        $(function () {
          $("#search").click(function(){
            
            var search_words = $('input[id=search_words]');
            
            var url = "<?php echo U('Home/Search/index/','','');?>";
            url += "&keywords=";
            url += search_words.val();
            //alert(url);
            window.location.href=url;

          });
        });
  </script>
  <script>
    var _hmt = _hmt || [];
    (function() {
      var hm = document.createElement("script");
      hm.src = "//hm.baidu.com/hm.js?225b31747f665ec848cbba3d4d9066e5";
      var s = document.getElementsByTagName("script")[0]; 
      s.parentNode.insertBefore(hm, s);
    })();
  </script>
 

  <meta name="description" content=<?php echo ($description); ?> /> 
  <meta name="keywords" content=<?php echo ($keywords); ?> /> 
  <title><?php echo ($title); ?></title> 

 </head> 
 <body> 

  <div id="navbar-top" class="navbar-top navbar navbar-default navbar-fixed-top"> 
   <div class="container"> 
    <div class="navbar-header"> 
     <a href="<?php echo U('Index/index');?>" class="navbar-brand">首页</a> 
     <a href="<?php echo U('About/index');?>" class="navbar-brand">关于</a> 
     <!--<img src="/assets/img/bl.png">--> 
    </div> 
    <p class="navbar-text navbar-right co-brand hidden-xs"><img src="/__debug/Public/images/logo2.png" /></p> 
   </div> 
  </div> 
  <header id="top-header" class="top-header jumbotron" style="background-image: url(/__debug/Public/images/header.jpg);"> 
   <div class="container"> 
    <div class="row"> 
     <div class="col-md-12"> 
      <div class="page-header"> 
       <h2>上翘课网，找工作不用愁<small> 职业从此与众不同</small></h2> 
       <p></p> 
       <form class="bs-example bs-example-form" role="form"> 
        <div class="row"> 
         <div class="col-lg-6 visible-lg"> 
          <div class="input-group "> 
           <input type="text" class="form-control" id="search_words" /> 
           <span class="input-group-btn"> <button class="btn btn-success" type="button" id="search"> 搜索职位 </button> </span> 
          </div>
          <!-- /input-group --> 
         </div>
         <!-- /.col-lg-6 --> 
        </div>
        <!-- /.row --> 
       </form> 
      </div> 
      <div class="features media"> 
       <h5 class="media-left">快捷入口：</h5> 
       <div class="media-body"> 
        <p class="coms"> <a target="_blank" href="#" class="label label-default label-ali" onclick="_hmt.push(['_trackEvent', 'coms', 'click', '阿里'])">阿里</a> 
         <!-- <a href="/company/腾讯" class="label label-default label-tencent" onclick="_hmt.push(['_trackEvent', 'coms', 'click', '腾讯'])">腾讯</a> --> <a target="_blank" href="#" class="label label-default label-baidu" onclick="_hmt.push(['_trackEvent', 'coms', 'click', '百度'])">百度</a> <a target="_blank" href="#" class="label label-default label-sina" onclick="_hmt.push(['_trackEvent', 'coms', 'click', '金山'])">金山</a> <a target="_blank" href="#" class="label label-default label-sohu" onclick="_hmt.push(['_trackEvent', 'coms', 'click', '迅雷'])">迅雷</a> <a target="_blank" href="#" class="label label-default label-360" onclick="_hmt.push(['_trackEvent', 'coms', 'click', '360'])">360</a> 
         <!-- <a href="/company/小米" class="label label-default label-xiaomi" onclick="_hmt.push(['_trackEvent', 'coms', 'click', '小米'])">小米</a> --> <a target="_blank" href="#" class="label label-default label-lenovo" onclick="_hmt.push(['_trackEvent', 'coms', 'click', '畅游'])">畅游</a> </p> 
        <p class="salary"> <a href="#" class="label label-default" onclick="_hmt.push(['_trackEvent', 'salary', 'click', '10万起'])">10万起</a> <a href="#" class="label label-default" onclick="_hmt.push(['_trackEvent', 'salary', 'click', '20万起'])">20万起</a> <a href="#" class="label label-default" onclick="_hmt.push(['_trackEvent', 'salary', 'click', '30万起'])">30万起</a> <a href="#" class="label label-default" onclick="_hmt.push(['_trackEvent', 'salary', 'click', '50万起'])">50万起</a> <a href="#" class="label label-default" onclick="_hmt.push(['_trackEvent', 'salary', 'click', '土豪'])">土豪</a> </p> 
       </div> 
      </div> 
     </div> 
    </div> 
   </div> 
  </header> 
  
 

  <section class="job-list"> 
   <div class="container"> 
    <div class="row"> 
     <h2>关于 <strong>翘课网</strong></h2>
<img src="/__debug/Public/images/about1.jpg" width="228px" height="168px"/>
     <h3>翘课网<small>是一个智能教育推荐平台<br>通过数据挖掘搭起企业招聘职位信息与职业教育培训的桥梁，将招聘与教育无缝结合，提高用户学习的针对性和动力，发挥互联网在提升职业教育上的效率优势。</small>
      </h3>
   <p></p>
      <h3>
        翘课网
        <small>是基于数据挖掘、机器学习的技术驱动的职业教育学习平台
          <br>通过建立起企业招聘信息与线上、线下职业教育的桥梁关系，让用户能够有针对性地学习，提高就业率<br>

</small>
      </h3>
      <p><img src="/__debug/Public/images/readme.png" width="328px" height="228px"/>阅读 源于 对知识的渴望！ 有目的的 持之以恒 的学习 将让你受益终生......
      </p>
      <p>写作 创造 深远的智慧，成败有时，只在于你对细节的把握，在需要抉择的时候做最优的选择<img src="/__debug/Public/images/xuexi.jpg" width="428px" height="428px"/>
      </p>
    </div>
    
   </div>
   <!-- .container --> 
  </section> 
  
  <footer class="footer " style="background-image: url(/__debug/Public/images/footer.jpg);"> 
   <div class="container"> 
    <div class="row footer-top"> 
     <div class="col-sm-6 col-lg-6"> 
      <h4> <img src="/__debug/Public/images/logo.png" /> </h4> 
      <h4><a href="#">翘课网</a><small>提供最精致的职业教育推荐服务</small></h4> 
      <p>通向职业 之路&nbsp;&nbsp;<img src="/__debug/Public/images/marker_red_sprite.png"/></p>
     </div> 
     <div class="col-sm-6  col-lg-5 col-lg-offset-1"> 
      <div class="row about"> 
       <div class="col-xs-4 col-md-3"> 
        <h4>关于</h4> 
        <ul class="list-unstyled"> 
         <li><a href="#">关于我们</a></li> 
         <li><a href="#">广告合作</a></li> 
         <li><a href="#">友情链接</a></li> 
        </ul> 
       </div> 
       <div class="col-xs-4 col-md-3"> 
        <h4>联系方式</h4> 
        <ul class="list-unstyled"> 
         <li><a href="http://weibo.com/warlife" title="逃课网官方微博" target="_blank">新浪微博</a></li> 
         <li><a href="mailto:justastriver@qq.com">电子邮件</a></li> 
        </ul> 
       </div> 
       <div class="col-xs-4 col-md-3"> 
        <h4>旗下网站</h4> 
        <ul class="list-unstyled"> 
         <li><a href="http://www.ren58.com/" target="_blank">人物世界</a></li> 
         <li><a href="http://www.pipichong.com/" target="_blank">爱美社区</a></li> 
        </ul> 
       </div> 
       <div class="col-md-3 hidden-xs"> 
        <h4>赞助商</h4> 
        <ul class="list-unstyled"> 
         <li><a href="http://www.baidu.cn/" target="_blank">百度</a></li> 
         <li><a href="https://www.ren58.com/" target="_blank">佳职联诚官网</a></li> 
        </ul> 
       </div> 
      </div> 
     </div> 
    </div> 
    <hr /> 
    <div class="row footer-bottom"> 
     <ul class="list-inline text-center"> 
      <li><a href="http://www.miibeian.gov.cn/" target="_blank">京ICP备14006962号-2</a></li> 
     </ul> 
    </div> 
   </div> 
  </footer> 
  <script src="./js/jquery.min.js"></script> 
  <script src="./js/bootstrap.min.js"></script> 
  <script src="./js/jquery.scrollUp.min.js"></script> 
  <script>
      ;(function(window, document, $){
        // $(document).ready(function(){
        //   $.adaptiveBackground.run()
        // });

        $.scrollUp({
              scrollName: 'scrollUp', // Element ID
              topDistance: '300', // Distance from top before showing element (px)
              topSpeed: 300, // Speed back to top (ms)
              animation: 'fade', // Fade, slide, none
              animationInSpeed: 200, // Animation in speed (ms)
              animationOutSpeed: 200, // Animation out speed (ms)
              scrollText: '<i class="fa fa-angle-up"></i>', // Text for element
              activeOverlay: false  // Set CSS color to display scrollUp active point, e.g '#00FFFF'
        });

        $(window).scroll(function() {
            if ($("#navbar-top").offset().top > 300) {
                $('.co-brand > img').attr('src', '/__debug/Public/images/logo2.png');
            } else {
                $('.co-brand > img').attr('src', '/__debug/Public/images/logo2.png');
            }
        });
      })(window, document, jQuery)
      </script>
  <a id="scrollUp" href="#" style="position: fixed; z-index: 2147483647; display: none;"> <i class="fa fa-angle-up"></i> </a> 
  <!--
      <script type="text/javascript">
      var _bdhmProtocol = (("https:" == document.location.protocol) ? " https://" : " http://");
      document.write(unescape("%3Cscript src='" + _bdhmProtocol + "hm.baidu.com/h.js%3Fd979e0116b6882c9cdc4cf2c8467d312' type='text/javascript'%3E%3C/script%3E"));
      </script>
      <script src="./js/h.js" type="text/javascript"></script>
      --> 

 </body>
</html>