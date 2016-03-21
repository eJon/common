<?php
namespace Home\Controller;
use Think\Controller;
class AboutController extends Controller {
    public function index(){
    	$this->assign('title','翘课网，让找工作变得简单--关于 翘课网');
    	$this->assign('keywords','翘课网,找工作,在线教育,招聘,求职');
    	$this->assign('description','翘课网，让找工作变得简单');
    	$this->display();
    }
}
?>